#ifndef _MSC_VER
#define RC_MEMORY_GUARD_LINT 1
#endif

#include "DEBUGMEMORYGUARD.H"

#ifndef RC_MEMORY_GUARD_LINT
#error "memory_guard_stub must be compiled with _MSC_VER undefined"
#endif

int main()
{
  DebugMemoryGuard::Options options;
  DebugMemoryGuard::Initialise(options);
  DebugMemoryGuard::Enable("lint");
  DebugMemoryGuard::Disable("lint");
  DebugMemoryGuard::SetEnabled(false, "lint");
  auto stats = DebugMemoryGuard::GetStatistics();
  DebugMemoryGuard::LogEvent("lint", "ok");
  DebugMemoryGuard::Shutdown();
  return DebugMemoryGuard::IsEnabled() ? 0 : static_cast<int>(stats.outstandingAllocations);
}
