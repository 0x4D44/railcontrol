/* DEBUGMEMORYGUARD.H
 *  Memory guard scaffolding for MSVC builds.
 */
#ifndef DEBUGMEMORYGUARD_H_INCLUDED
#define DEBUGMEMORYGUARD_H_INCLUDED

#include <cstddef>

class DebugMemoryGuard
{
public:
#if defined(_MSC_VER)
  struct Options
  {
    bool enableGuard = true;
    std::size_t maxFileBytes = 2u * 1024u * 1024u;
    std::size_t maxFileCount = 5u;
  };

  struct Statistics
  {
    std::size_t outstandingAllocations = 0;
    std::size_t outstandingBytes = 0;
    std::size_t peakAllocations = 0;
    std::size_t peakBytes = 0;
    bool handlersActive = false;
    bool guardEnabled = false;
  };

  static void Initialise(const Options& options);
  static void Shutdown();
  static bool IsEnabled();
  static void Enable(const char* reason = nullptr);
  static void Disable(const char* reason = nullptr);
  static void SetEnabled(bool enable, const char* reason = nullptr);
  static Statistics GetStatistics();
  static void LogEvent(const char* category, const char* message);
  static const char* GetLogDirectory();
#else
  struct Options
  {
    bool enableGuard = false;
    std::size_t maxFileBytes = 0;
    std::size_t maxFileCount = 0;
  };

  struct Statistics
  {
    std::size_t outstandingAllocations = 0;
    std::size_t outstandingBytes = 0;
    std::size_t peakAllocations = 0;
    std::size_t peakBytes = 0;
    bool handlersActive = false;
    bool guardEnabled = false;
  };

  static void Initialise(const Options&) {}
  static void Shutdown() {}
  static bool IsEnabled() { return false; }
  static void Enable(const char* = nullptr) {}
  static void Disable(const char* = nullptr) {}
  static void SetEnabled(bool, const char* = nullptr) {}
  static Statistics GetStatistics() { return Statistics(); }
  static void LogEvent(const char*, const char*) {}
  static const char* GetLogDirectory() { return ""; }
#endif
};

#endif // DEBUGMEMORYGUARD_H_INCLUDED
