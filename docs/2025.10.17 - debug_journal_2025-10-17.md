# Debug Journal — 2025-10-17

## 22:58
- Reviewed `progress_summary_runtime.txt` to confirm current modernization status and outstanding crash investigation tasks.
- Noted SentinelOne hook suspicion and existing CDB traces in repo root. Planning to rebuild with PDBs and capture fresh debugger output tonight.

## 23:16
- Ran `build_msvc.bat Debug` to produce a fresh MSVC Debug binary (`build/msvc/Debug/railc_msvc.exe`) and verified that `railc_msvc.pdb` exists for symbolized debugging.

## 23:28
- Automated a CDB capture via `/mnt/c/.../cdb.exe` with `cdb_cmds_capture.txt`, logging to `cdb_capture.log`.
- Breakpoint printouts show `owl::TWindow::PerformSetupAndTransfer` receiving a sane `this` pointer, but the virtual call dispatches to `eax=00de76dc`, which CDB flags as an invalid address when dereferenced.
- Stack trace confirms SentinelOne's `InProcessClient32.dll` is injected between `CreateWindowExA` and OWL window creation, supporting the hypothesis that its hook is corrupting the vtable slot (call into address 0x00de76dc).

## 23:36
- Collected a second CDB run with `cdb_cmds_vtable.txt` (`cdb_vtable.log`) to dump the vtable target for slot `0x80` and watch subsequent invocations.
- Observed multiple successful entries where `this` pointed into our `railc_msvc` heap, but the crashing pass shows `ecx/esi` replaced with `0x77873680` (inside `ntdll`) before the `or [esi+70h], 20h` write, proving the object pointer is clobbered post hook.
- Reinforced that the access violation comes from writing to `esi+0x70` after SentinelOne intercepts the `CreateWindowExA` call chain (`InProcessClient32` frames) and returns a bogus pointer.

## 23:52
- Scripted `cdb_cmds_slot80.txt` to log the slot `0x80` target and call stack; confirmed it jumps through the import thunk to `TMainWindow::SetupWindow` and captured `cdb_slot80.log`.
- Ran focused session (`cdb_cmds_tmwin.txt`) to trace `TMainWindow::SetupWindow` entry/exit alongside `PerformSetupAndTransfer`. Entry shows `esi` is initialized correctly and the RET stub points at `0x00cb0650`, but the post-call breakpoint still reports `esi` scrubbed to `0x7787xxxx`, narrowing the corruption window to “after callee returns, before OWL flags wfFullyCreated`.
- Overall evidence now implicates a foreign hook (SentinelOne) stomping preserved registers during window creation; next step is to reproduce on an environment without the agent or secure an exemption.

## 00:07
- Created `cdb_cmds_esi_trace.txt` to single-step the virtual call and break inside `InProcessClient32.dll`. Logged output in `cdb_esi_trace.log`.
- Breakpoints on `InProcessClient32+0x91161/0x912f6` show the Sentinel hook exiting with `esi` rewritten to an address inside the Sentinel module (`0x74e01560` on this run), confirming the non-volatile register clobber occurs before control returns to OWL.
- Demonstrated that when the hook is skipped (child windows prior to the main frame), `esi` remains stable through `TMainWindow::SetupWindow`, further isolating the crash to Sentinel’s instrumentation and not our code.

## 00:18
- Patched `TWindow::PerformSetupAndTransfer` in `third_party/owlnext/source/owlcore/window.cpp` to cache `this` before calling `SetupWindow()` and reuse the saved pointer for `SetFlag`/`TransferData`.
- Rationale: MSVC previously held the pointer in `ESI`; SentinelOne’s hook corrupts that register post-call. Using the saved pointer forces the compiler to reload from the stack, providing a temporary runtime guard while security works on disabling the hook.

## 00:27
- Attempted to enforce the guard via a helper wrapper (`FinalizeWindowSetup`), but disassembly (`u railc_msvc!owl::TWindow::PerformSetupAndTransfer`) showed the compiler still emitted the original `mov esi, ecx` pattern and SentinelOne continued to crash the app.
- Reverted the OWL source changes to avoid carrying unused edits; the mitigation must focus on disabling the agent or injecting a runtime check elsewhere (e.g., separate diagnostic DLL) rather than altering third-party sources.

## 00:34
- Tried again using a `volatile` pointer and eventually a `__declspec(noinline)` helper (`FinalizeWindowAfterSetup`) to force MSVC to reload the preserved `this` pointer from memory.
- Post-build disassembly remained unchanged (`or [esi+70h], 20h`), confirming the compiler adheres to ThisCall conventions regardless of local helpers; rolled back to the stock OWL implementation to keep the codebase clean.
