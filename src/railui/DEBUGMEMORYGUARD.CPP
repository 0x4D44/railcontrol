/* DEBUGMEMORYGUARD.CPP
 *  Memory guard diagnostics for MSVC builds.
 */

#include "classdef.h"
#include "DEBUGMEMORYGUARD.H"

#if defined(_MSC_VER)

#include <windows.h>
#include <DbgHelp.h>
#include <strsafe.h>

#include <atomic>
#include <cstddef>
#include <cstring>
#include <malloc.h>
#include <mutex>
#include <new>
#include <new.h>
#include <string>
#include <unordered_map>

#include <intrin.h>
#pragma intrinsic(_ReturnAddress)

namespace
{
  constexpr std::size_t kDefaultMaxFileBytes = 2u * 1024u * 1024u;
  constexpr std::size_t kDefaultMaxFileCount = 5u;
  constexpr std::size_t kMaxSummaryEntries = 5u;

  using MiniDumpWriteDumpFn = BOOL (WINAPI *)(HANDLE,
                                              DWORD,
                                              HANDLE,
                                              MINIDUMP_TYPE,
                                              PMINIDUMP_EXCEPTION_INFORMATION,
                                              PMINIDUMP_USER_STREAM_INFORMATION,
                                              PMINIDUMP_CALLBACK_INFORMATION);

  struct AllocationInfo
  {
    std::size_t size = 0;
    void* returnAddress = nullptr;
    unsigned long long id = 0;
    DWORD threadId = 0;
  };

  struct OutstandingEntry
  {
    void* pointer = nullptr;
    AllocationInfo info;
  };

  struct GuardState
  {
    bool initialised = false;
    std::atomic<bool> enabled{false};
    std::size_t maxFileBytes = kDefaultMaxFileBytes;
    std::size_t maxFileCount = kDefaultMaxFileCount;

    std::mutex controlMutex;
    std::mutex trackingMutex;
    std::mutex writeMutex;

    std::unordered_map<void*, AllocationInfo> allocations;
    unsigned long long nextAllocationId = 0;
    std::size_t outstandingBytes = 0;
    std::size_t outstandingAllocations = 0;
    std::size_t peakBytes = 0;
    std::size_t peakAllocations = 0;
    std::size_t totalAllocations = 0;
    std::size_t totalFrees = 0;
    std::size_t sessionAllocations = 0;

    std::string logDirectory;
    std::string logBasePath;
    std::string dumpDirectory;

    HMODULE dbghelpModule = nullptr;
    MiniDumpWriteDumpFn miniDumpWriteDump = nullptr;
    bool dbghelpAttempted = false;

    PVOID vectoredHandle = nullptr;
    LPTOP_LEVEL_EXCEPTION_FILTER previousFilter = nullptr;
    bool handlersActive = false;

    bool dumpWritten = false;
  };

  GuardState& GetState()
  {
    static GuardState sState;
    return sState;
  }

  void AppendPathLeaf(std::string& path, const char* leaf)
  {
    if (!leaf || leaf[0] == '\0')
    {
      return;
    }

    if (!path.empty() && path.back() != '\\')
    {
      path.append("\\");
    }
    path.append(leaf);
  }

  std::string ExtractDirectory(const char* fullPath)
  {
    if (!fullPath)
    {
      return std::string();
    }

    std::string result(fullPath);
    const std::size_t pos = result.find_last_of("\\/");
    if (pos != std::string::npos)
    {
      result.assign(result.c_str(), pos);
    }
    else
    {
      result.clear();
    }
    return result;
  }

  std::string BuildArchivePath(const std::string& basePath, std::size_t index)
  {
    std::string archivePath(basePath);
    archivePath.append(".");
    archivePath.append(std::to_string(index));
    return archivePath;
  }

  ULONGLONG QueryFileSize(const std::string& path)
  {
    if (path.empty())
    {
      return 0;
    }

    WIN32_FILE_ATTRIBUTE_DATA data = {};
    if (!::GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &data))
    {
      return 0;
    }

    ULARGE_INTEGER size = {};
    size.HighPart = data.nFileSizeHigh;
    size.LowPart = data.nFileSizeLow;
    return size.QuadPart;
  }

  void RotateLogsIfNeeded(GuardState& state, std::size_t nextWriteBytes, bool force)
  {
    if (state.logBasePath.empty())
    {
      return;
    }

    if (!force && !state.enabled.load())
    {
      return;
    }

    if (state.maxFileCount == 0)
    {
      return;
    }

    const ULONGLONG currentSize = QueryFileSize(state.logBasePath);
    const ULONGLONG threshold = static_cast<ULONGLONG>(state.maxFileBytes);

    if ((threshold == 0) || (currentSize + nextWriteBytes) <= threshold)
    {
      return;
    }

    for (std::size_t idx = state.maxFileCount; idx > 0; --idx)
    {
      const std::string src = (idx == 1)
                                ? state.logBasePath
                                : BuildArchivePath(state.logBasePath, idx - 1);
      const std::string dst = BuildArchivePath(state.logBasePath, idx);

      if (::GetFileAttributesA(src.c_str()) == INVALID_FILE_ATTRIBUTES)
      {
        continue;
      }

      ::DeleteFileA(dst.c_str());
      ::MoveFileExA(src.c_str(), dst.c_str(), MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING);
    }
  }

  void WriteLineLocked(GuardState& state, const char* text, bool force)
  {
    if (!text || state.logBasePath.empty())
    {
      return;
    }

    if (!force && !state.enabled.load())
    {
      return;
    }

    const std::size_t length = std::strlen(text);
    const std::size_t writeBytes = length + 2;
    RotateLogsIfNeeded(state, writeBytes, force);

    HANDLE file = ::CreateFileA(state.logBasePath.c_str(),
                                FILE_APPEND_DATA,
                                FILE_SHARE_READ,
                                nullptr,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                nullptr);
    if (file == INVALID_HANDLE_VALUE)
    {
      return;
    }

    DWORD written = 0;
    ::WriteFile(file, text, static_cast<DWORD>(length), &written, nullptr);
    ::WriteFile(file, "\r\n", 2, &written, nullptr);
    ::CloseHandle(file);
  }

  void LogFormatted(GuardState& state, const char* category, const char* message, bool force)
  {
    if (!state.initialised)
    {
      return;
    }

    if (!force && !state.enabled.load())
    {
      return;
    }

    SYSTEMTIME now = {};
    ::GetLocalTime(&now);

    char formatted[512];
    if (category && category[0] != '\0')
    {
      ::StringCchPrintfA(formatted,
                         ARRAYSIZE(formatted),
                         "%04u-%02u-%02u %02u:%02u:%02u.%03u [%s] %s",
                         now.wYear,
                         now.wMonth,
                         now.wDay,
                         now.wHour,
                         now.wMinute,
                         now.wSecond,
                         now.wMilliseconds,
                         category,
                         message ? message : "");
    }
    else
    {
      ::StringCchPrintfA(formatted,
                         ARRAYSIZE(formatted),
                         "%04u-%02u-%02u %02u:%02u:%02u.%03u %s",
                         now.wYear,
                         now.wMonth,
                         now.wDay,
                         now.wHour,
                         now.wMinute,
                         now.wSecond,
                         now.wMilliseconds,
                         message ? message : "");
    }

    std::lock_guard<std::mutex> guard(state.writeMutex);
    WriteLineLocked(state, formatted, force);
  }

  void LogLifecycleEvent(GuardState& state, const char* category, const char* message)
  {
    LogFormatted(state, category, message, true);
  }

  bool LoadDbgHelp(GuardState& state)
  {
    if (state.miniDumpWriteDump)
    {
      return true;
    }

    if (state.dbghelpAttempted)
    {
      return false;
    }

    state.dbghelpAttempted = true;

    HMODULE module = ::GetModuleHandleA("dbghelp.dll");
    if (!module)
    {
      module = ::LoadLibraryA("dbghelp.dll");
    }

    if (!module)
    {
      LogLifecycleEvent(state, "exception", "dbghelp_load_failed err=module_not_found");
      return false;
    }

    FARPROC proc = ::GetProcAddress(module, "MiniDumpWriteDump");
    if (!proc)
    {
      LogLifecycleEvent(state, "exception", "dbghelp_missing_minidumpwritedump");
      ::FreeLibrary(module);
      return false;
    }

    state.dbghelpModule = module;
    state.miniDumpWriteDump = reinterpret_cast<MiniDumpWriteDumpFn>(proc);
    LogLifecycleEvent(state, "exception", "dbghelp_loaded");
    return true;
  }

  std::string SanitizeTrigger(const char* trigger)
  {
    if (!trigger || trigger[0] == '\0')
    {
      return std::string("unknown");
    }

    std::string result(trigger);
    for (char& ch : result)
    {
      if ((ch >= 'a' && ch <= 'z') ||
          (ch >= 'A' && ch <= 'Z') ||
          (ch >= '0' && ch <= '9'))
      {
        continue;
      }
      ch = '_';
    }
    return result;
  }

  std::string BuildDumpFilePath(GuardState& state, const char* trigger)
  {
    if (state.dumpDirectory.empty())
    {
      return std::string();
    }

    ::CreateDirectoryA(state.dumpDirectory.c_str(), nullptr);

    SYSTEMTIME now = {};
    ::GetLocalTime(&now);

    std::string safeTrigger = SanitizeTrigger(trigger);

    char fileName[160];
    ::StringCchPrintfA(fileName,
                       ARRAYSIZE(fileName),
                       "railcontrol_%04u%02u%02u_%02u%02u%02u_%lu_%s.dmp",
                       now.wYear,
                       now.wMonth,
                       now.wDay,
                       now.wHour,
                       now.wMinute,
                       now.wSecond,
                       ::GetCurrentProcessId(),
                       safeTrigger.c_str());

    std::string path = state.dumpDirectory;
    AppendPathLeaf(path, fileName);
    return path;
  }

  bool WriteMiniDump(GuardState& state, EXCEPTION_POINTERS* pointers, const char* trigger)
  {
    if (!state.enabled.load())
    {
      return false;
    }

    if (!LoadDbgHelp(state))
    {
      return false;
    }

    std::string dumpPath = BuildDumpFilePath(state, trigger);
    if (dumpPath.empty())
    {
      return false;
    }

    HANDLE dumpFile = ::CreateFileA(dumpPath.c_str(),
                                    GENERIC_WRITE,
                                    FILE_SHARE_READ,
                                    nullptr,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    nullptr);
    if (dumpFile == INVALID_HANDLE_VALUE)
    {
      char message[256];
      ::StringCchPrintfA(message,
                         ARRAYSIZE(message),
                         "minidump_open_failed err=%lu path=%s",
                         ::GetLastError(),
                         dumpPath.c_str());
      LogLifecycleEvent(state, "exception", message);
      return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo = {};
    exceptionInfo.ThreadId = ::GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = pointers;
    exceptionInfo.ClientPointers = FALSE;

    const MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
      MiniDumpWithHandleData |
      MiniDumpWithThreadInfo |
      MiniDumpWithIndirectlyReferencedMemory);

    BOOL wroteDump = state.miniDumpWriteDump(::GetCurrentProcess(),
                                             ::GetCurrentProcessId(),
                                             dumpFile,
                                             dumpType,
                                             pointers ? &exceptionInfo : nullptr,
                                             nullptr,
                                             nullptr);
    ::CloseHandle(dumpFile);

    if (wroteDump)
    {
      state.dumpWritten = true;
      char message[256];
      ::StringCchPrintfA(message,
                         ARRAYSIZE(message),
                         "minidump_success path=%s",
                         dumpPath.c_str());
      LogLifecycleEvent(state, "exception", message);
      return true;
    }

    char message[256];
    ::StringCchPrintfA(message,
                       ARRAYSIZE(message),
                       "minidump_failed err=%lu path=%s",
                       ::GetLastError(),
                       dumpPath.c_str());
    LogLifecycleEvent(state, "exception", message);
    ::DeleteFileA(dumpPath.c_str());
    return false;
  }

  LONG WINAPI GuardVectoredHandler(EXCEPTION_POINTERS* pointers);
  LONG WINAPI GuardUnhandledHandler(EXCEPTION_POINTERS* pointers);

  void InstallHandlersLocked(GuardState& state)
  {
    if (state.handlersActive)
    {
      return;
    }

    state.vectoredHandle = ::AddVectoredExceptionHandler(1, GuardVectoredHandler);
    state.previousFilter = ::SetUnhandledExceptionFilter(GuardUnhandledHandler);
    state.handlersActive = true;

    if (!state.vectoredHandle)
    {
      LogLifecycleEvent(state, "guard", "vectored_handler_install_failed");
    }
  }

  void RemoveHandlersLocked(GuardState& state)
  {
    if (!state.handlersActive)
    {
      return;
    }

    if (state.vectoredHandle)
    {
      ::RemoveVectoredExceptionHandler(state.vectoredHandle);
      state.vectoredHandle = nullptr;
    }
    else
    {
      LogLifecycleEvent(state, "guard", "vectored_handler_missing_on_remove");
    }

    ::SetUnhandledExceptionFilter(state.previousFilter);
    state.previousFilter = nullptr;
    state.handlersActive = false;
  }

  void LogOutstandingSummary(GuardState& state, const char* phase)
  {
    OutstandingEntry samples[kMaxSummaryEntries] = {};
    std::size_t sampleCount = 0;
    std::size_t outstandingAllocations = 0;
    std::size_t outstandingBytes = 0;
    std::size_t totalAllocations = 0;
    std::size_t totalFrees = 0;
    std::size_t peakAllocations = 0;
    std::size_t peakBytes = 0;

    {
      std::lock_guard<std::mutex> lock(state.trackingMutex);
      outstandingAllocations = state.outstandingAllocations;
      outstandingBytes = state.outstandingBytes;
      totalAllocations = state.totalAllocations;
      totalFrees = state.totalFrees;
      peakAllocations = state.peakAllocations;
      peakBytes = state.peakBytes;

      if (outstandingAllocations > 0)
      {
        for (auto it = state.allocations.begin();
             (it != state.allocations.end()) && (sampleCount < kMaxSummaryEntries);
             ++it)
        {
          samples[sampleCount].pointer = it->first;
          samples[sampleCount].info = it->second;
          ++sampleCount;
        }
      }
    }

    char header[256];
    ::StringCchPrintfA(header,
                       ARRAYSIZE(header),
                       "%s outstanding=%zu bytes=%zu total_alloc=%zu total_free=%zu peak=%zu/%zu",
                       phase ? phase : "guard",
                       outstandingAllocations,
                       outstandingBytes,
                       totalAllocations,
                       totalFrees,
                       peakAllocations,
                       peakBytes);
    LogLifecycleEvent(state, "guard", header);

    for (std::size_t idx = 0; idx < sampleCount; ++idx)
    {
      const OutstandingEntry& entry = samples[idx];
      char line[256];
      ::StringCchPrintfA(line,
                         ARRAYSIZE(line),
                         "outstanding ptr=%p id=%llu size=%zu caller=%p tid=%lu",
                         entry.pointer,
                         entry.info.id,
                         entry.info.size,
                         entry.info.returnAddress,
                         entry.info.threadId);
      LogLifecycleEvent(state, "guard", line);
    }
  }

  void SetEnabledLocked(GuardState& state, bool enable, const char* reason)
  {
    const bool current = state.enabled.load();
    if (enable == current)
    {
      return;
    }

    if (enable)
    {
      state.dumpWritten = false;
      state.sessionAllocations = 0;
      state.enabled.store(true);
      InstallHandlersLocked(state);

      char message[256];
      ::StringCchPrintfA(message,
                         ARRAYSIZE(message),
                         "memory guard enabled%s%s",
                         (reason && reason[0] != '\0') ? " reason=" : "",
                         (reason && reason[0] != '\0') ? reason : "");
      LogLifecycleEvent(state, "guard", message);
      return;
    }

    LogOutstandingSummary(state, "guard_disable");

    char message[256];
    ::StringCchPrintfA(message,
                       ARRAYSIZE(message),
                       "memory guard disabled%s%s",
                       (reason && reason[0] != '\0') ? " reason=" : "",
                       (reason && reason[0] != '\0') ? reason : "");
    LogLifecycleEvent(state, "guard", message);

    state.enabled.store(false);
    RemoveHandlersLocked(state);
  }

  thread_local bool gAllocationReentry = false;

  class RecursionGuard
  {
  public:
    RecursionGuard()
    {
      if (gAllocationReentry)
      {
        mBypass = true;
      }
      else
      {
        gAllocationReentry = true;
      }
    }

    ~RecursionGuard()
    {
      if (!mBypass)
      {
        gAllocationReentry = false;
      }
    }

    bool ShouldBypass() const
    {
      return mBypass;
    }

  private:
    bool mBypass = false;
  };

  void RecordAllocation(void* memory, std::size_t size, void* caller)
  {
    if (!memory)
    {
      return;
    }

    RecursionGuard guard;
    if (guard.ShouldBypass())
    {
      return;
    }

    GuardState& state = GetState();
    if (!state.initialised || !state.enabled.load())
    {
      return;
    }

    AllocationInfo info;
    std::size_t outstandingAllocations = 0;
    std::size_t outstandingBytes = 0;
    std::size_t totalAllocations = 0;

    {
      std::lock_guard<std::mutex> lock(state.trackingMutex);
      info.id = ++state.nextAllocationId;
      info.size = size;
      info.returnAddress = caller;
      info.threadId = ::GetCurrentThreadId();

      state.allocations[memory] = info;
      state.outstandingBytes += size;
      state.outstandingAllocations += 1;
      state.totalAllocations += 1;
      state.sessionAllocations += 1;

      if (state.outstandingBytes > state.peakBytes)
      {
        state.peakBytes = state.outstandingBytes;
      }
      if (state.outstandingAllocations > state.peakAllocations)
      {
        state.peakAllocations = state.outstandingAllocations;
      }

      outstandingAllocations = state.outstandingAllocations;
      outstandingBytes = state.outstandingBytes;
      totalAllocations = state.totalAllocations;
    }

    char message[256];
    ::StringCchPrintfA(message,
                       ARRAYSIZE(message),
                       "alloc id=%llu ptr=%p size=%zu caller=%p tid=%lu outstanding=%zu bytes=%zu total=%zu",
                       info.id,
                       memory,
                       size,
                       info.returnAddress,
                       info.threadId,
                       outstandingAllocations,
                       outstandingBytes,
                       totalAllocations);
    DebugMemoryGuard::LogEvent("alloc", message);
  }

  void RecordFree(void* memory, void* caller, std::size_t reportedSize)
  {
    if (!memory)
    {
      return;
    }

    RecursionGuard guard;
    if (guard.ShouldBypass())
    {
      return;
    }

    GuardState& state = GetState();
    if (!state.initialised || !state.enabled.load())
    {
      return;
    }

    AllocationInfo info;
    bool found = false;
    std::size_t outstandingAllocations = 0;
    std::size_t outstandingBytes = 0;
    std::size_t totalFrees = 0;
    std::size_t sessionAllocations = 0;

    {
      std::lock_guard<std::mutex> lock(state.trackingMutex);
      auto it = state.allocations.find(memory);
      if (it != state.allocations.end())
      {
        info = it->second;
        state.allocations.erase(it);
        if (state.outstandingBytes >= info.size)
        {
          state.outstandingBytes -= info.size;
        }
        else
        {
          state.outstandingBytes = 0;
        }
        if (state.outstandingAllocations > 0)
        {
          state.outstandingAllocations -= 1;
        }

        state.totalFrees += 1;
        found = true;
      }

      outstandingAllocations = state.outstandingAllocations;
      outstandingBytes = state.outstandingBytes;
      totalFrees = state.totalFrees;
      sessionAllocations = state.sessionAllocations;
    }

    if (found)
    {
      char message[256];
      ::StringCchPrintfA(message,
                         ARRAYSIZE(message),
                         "free id=%llu ptr=%p caller=%p tid=%lu outstanding=%zu bytes=%zu total_free=%zu%s%s",
                         info.id,
                         memory,
                         caller,
                         ::GetCurrentThreadId(),
                         outstandingAllocations,
                         outstandingBytes,
                         totalFrees,
                         (reportedSize != 0 && reportedSize != info.size) ? " size_mismatch=" : "",
                         (reportedSize != 0 && reportedSize != info.size) ? "detected" : "");
      DebugMemoryGuard::LogEvent("free", message);
      return;
    }

    if (sessionAllocations == 0)
    {
      return;
    }

    char message[256];
    ::StringCchPrintfA(message,
                       ARRAYSIZE(message),
                       "free_unknown ptr=%p caller=%p tid=%lu size_hint=%zu",
                       memory,
                       caller,
                       ::GetCurrentThreadId(),
                       reportedSize);
    DebugMemoryGuard::LogEvent("anomaly", message);
  }

  LONG WINAPI GuardVectoredHandler(EXCEPTION_POINTERS* pointers)
  {
    GuardState& state = GetState();
    if (!state.initialised || !state.enabled.load())
    {
      return EXCEPTION_CONTINUE_SEARCH;
    }

    if (!pointers || !pointers->ExceptionRecord)
    {
      return EXCEPTION_CONTINUE_SEARCH;
    }

    const EXCEPTION_RECORD* record = pointers->ExceptionRecord;

    char message[256];
    ::StringCchPrintfA(message,
                       ARRAYSIZE(message),
                       "first_chance code=0x%08lX flags=0x%08lX address=%p",
                       record->ExceptionCode,
                       record->ExceptionFlags,
                       record->ExceptionAddress);
    LogLifecycleEvent(state, "exception", message);

    if ((record->ExceptionFlags & EXCEPTION_NONCONTINUABLE) != 0)
    {
      if (!state.dumpWritten)
      {
        WriteMiniDump(state, pointers, "first_chance");
      }
    }

    return EXCEPTION_CONTINUE_SEARCH;
  }

  LONG WINAPI GuardUnhandledHandler(EXCEPTION_POINTERS* pointers)
  {
    GuardState& state = GetState();
    if (!state.initialised || !state.enabled.load())
    {
      if (state.previousFilter)
      {
        return state.previousFilter(pointers);
      }
      return EXCEPTION_CONTINUE_SEARCH;
    }

    DWORD code = 0;
    void* address = nullptr;
    if (pointers && pointers->ExceptionRecord)
    {
      code = pointers->ExceptionRecord->ExceptionCode;
      address = pointers->ExceptionRecord->ExceptionAddress;
    }

    char message[256];
    ::StringCchPrintfA(message,
                       ARRAYSIZE(message),
                       "unhandled code=0x%08lX address=%p",
                       code,
                       address);
    LogLifecycleEvent(state, "exception", message);

    WriteMiniDump(state, pointers, "unhandled");

    if (state.previousFilter)
    {
      return state.previousFilter(pointers);
    }

    return EXCEPTION_EXECUTE_HANDLER;
  }

  void* GuardAllocate(std::size_t size, std::size_t alignment)
  {
    std::size_t actualSize = (size == 0) ? 1u : size;
    if (alignment < alignof(std::max_align_t))
    {
      alignment = alignof(std::max_align_t);
    }

    for (;;)
    {
      void* memory = _aligned_malloc(actualSize, alignment);
      if (memory)
      {
        return memory;
      }

      if (_callnewh(actualSize) == 0)
      {
        throw std::bad_alloc();
      }
    }
  }

  void GuardDeallocate(void* memory)
  {
    if (memory)
    {
      _aligned_free(memory);
    }
  }
}

void DebugMemoryGuard::Initialise(const Options& options)
{
  GuardState& state = GetState();
  std::lock_guard<std::mutex> guard(state.controlMutex);

  if (state.initialised)
  {
    state.maxFileBytes = (options.maxFileBytes != 0) ? options.maxFileBytes : kDefaultMaxFileBytes;
    state.maxFileCount = (options.maxFileCount != 0) ? options.maxFileCount : kDefaultMaxFileCount;
    SetEnabledLocked(state, options.enableGuard, "reinitialise");
    return;
  }

  state.initialised = true;
  state.maxFileBytes = (options.maxFileBytes != 0) ? options.maxFileBytes : kDefaultMaxFileBytes;
  state.maxFileCount = (options.maxFileCount != 0) ? options.maxFileCount : kDefaultMaxFileCount;

  const char* diagLogPath = GetLogFilePath();
  state.logDirectory = ExtractDirectory(diagLogPath);
  if (state.logDirectory.empty())
  {
    state.enabled.store(false);
    return;
  }

  ::CreateDirectoryA(state.logDirectory.c_str(), nullptr);

  state.logBasePath = state.logDirectory;
  AppendPathLeaf(state.logBasePath, "memory_guard.log");

  std::string baseDirectory = ExtractDirectory(state.logDirectory.c_str());
  if (!baseDirectory.empty())
  {
    ::CreateDirectoryA(baseDirectory.c_str(), nullptr);
  }

  state.dumpDirectory = baseDirectory;
  AppendPathLeaf(state.dumpDirectory, "Dumps");
  ::CreateDirectoryA(state.dumpDirectory.c_str(), nullptr);

  SetEnabledLocked(state, options.enableGuard, "initialise");
}

void DebugMemoryGuard::Shutdown()
{
  GuardState& state = GetState();
  std::lock_guard<std::mutex> guard(state.controlMutex);
  if (!state.initialised)
  {
    return;
  }

  SetEnabledLocked(state, false, "shutdown");
}

bool DebugMemoryGuard::IsEnabled()
{
  return GetState().enabled.load();
}

void DebugMemoryGuard::Enable(const char* reason)
{
  GuardState& state = GetState();
  std::lock_guard<std::mutex> guard(state.controlMutex);
  if (!state.initialised)
  {
    return;
  }
  SetEnabledLocked(state, true, reason);
}

void DebugMemoryGuard::Disable(const char* reason)
{
  GuardState& state = GetState();
  std::lock_guard<std::mutex> guard(state.controlMutex);
  if (!state.initialised)
  {
    return;
  }
  SetEnabledLocked(state, false, reason);
}

void DebugMemoryGuard::SetEnabled(bool enable, const char* reason)
{
  GuardState& state = GetState();
  std::lock_guard<std::mutex> guard(state.controlMutex);
  if (!state.initialised)
  {
    return;
  }
  SetEnabledLocked(state, enable, reason);
}

DebugMemoryGuard::Statistics DebugMemoryGuard::GetStatistics()
{
  GuardState& state = GetState();
  Statistics stats;

  stats.guardEnabled = state.enabled.load();
  stats.handlersActive = state.handlersActive;

  if (!state.initialised)
  {
    return stats;
  }

  std::lock_guard<std::mutex> lock(state.trackingMutex);
  stats.outstandingAllocations = state.outstandingAllocations;
  stats.outstandingBytes = state.outstandingBytes;
  stats.peakAllocations = state.peakAllocations;
  stats.peakBytes = state.peakBytes;
  return stats;
}

void DebugMemoryGuard::LogEvent(const char* category, const char* message)
{
  GuardState& state = GetState();
  LogFormatted(state, category, message, false);
}

const char* DebugMemoryGuard::GetLogDirectory()
{
  GuardState& state = GetState();
  return state.logDirectory.c_str();
}

void* operator new(std::size_t size)
{
  void* memory = GuardAllocate(size, alignof(std::max_align_t));
  RecordAllocation(memory, size, _ReturnAddress());
  return memory;
}

void* operator new[](std::size_t size)
{
  void* memory = GuardAllocate(size, alignof(std::max_align_t));
  RecordAllocation(memory, size, _ReturnAddress());
  return memory;
}

void* operator new(std::size_t size, std::nothrow_t const&) noexcept
{
  try
  {
    return ::operator new(size);
  }
  catch (...)
  {
    return nullptr;
  }
}

void* operator new[](std::size_t size, std::nothrow_t const&) noexcept
{
  try
  {
    return ::operator new[](size);
  }
  catch (...)
  {
    return nullptr;
  }
}

void* operator new(std::size_t size, std::align_val_t alignment)
{
  void* memory = GuardAllocate(size, static_cast<std::size_t>(alignment));
  RecordAllocation(memory, size, _ReturnAddress());
  return memory;
}

void* operator new[](std::size_t size, std::align_val_t alignment)
{
  void* memory = GuardAllocate(size, static_cast<std::size_t>(alignment));
  RecordAllocation(memory, size, _ReturnAddress());
  return memory;
}

void* operator new(std::size_t size, std::align_val_t alignment, std::nothrow_t const&) noexcept
{
  try
  {
    return ::operator new(size, alignment);
  }
  catch (...)
  {
    return nullptr;
  }
}

void* operator new[](std::size_t size, std::align_val_t alignment, std::nothrow_t const&) noexcept
{
  try
  {
    return ::operator new[](size, alignment);
  }
  catch (...)
  {
    return nullptr;
  }
}

void operator delete(void* memory) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

void operator delete[](void* memory) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

void operator delete(void* memory, std::size_t size) noexcept
{
  RecordFree(memory, _ReturnAddress(), size);
  GuardDeallocate(memory);
}

void operator delete[](void* memory, std::size_t size) noexcept
{
  RecordFree(memory, _ReturnAddress(), size);
  GuardDeallocate(memory);
}

void operator delete(void* memory, std::nothrow_t const&) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

void operator delete[](void* memory, std::nothrow_t const&) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

void operator delete(void* memory, std::align_val_t alignment) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

void operator delete[](void* memory, std::align_val_t alignment) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

void operator delete(void* memory, std::align_val_t alignment, std::size_t size) noexcept
{
  RecordFree(memory, _ReturnAddress(), size);
  GuardDeallocate(memory);
}

void operator delete[](void* memory, std::align_val_t alignment, std::size_t size) noexcept
{
  RecordFree(memory, _ReturnAddress(), size);
  GuardDeallocate(memory);
}

void operator delete(void* memory, std::align_val_t alignment, std::nothrow_t const&) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

void operator delete[](void* memory, std::align_val_t alignment, std::nothrow_t const&) noexcept
{
  RecordFree(memory, _ReturnAddress(), 0);
  GuardDeallocate(memory);
}

#endif // defined(_MSC_VER)
