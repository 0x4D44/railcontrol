# SentinelOne Hook Investigation (2025-10-17)

## Background
`railc_msvc.exe` crashes during startup inside `owl::TWindow::PerformSetupAndTransfer`, attempting to set the `wfFullyCreated` flag on the window instance. The failure occurs before the main frame paints, preventing any UI interaction.

Previous breakpoint runs (`cdb_capture.log`, `cdb_vtable.log`, `cdb_slot80.log`, `cdb_tmwin.log`) narrowed the issue to the virtual call dispatched through vtable slot `0x80`, which should invoke `TMainWindow::SetupWindow`. The call chain immediately follows a set of frames originating inside SentinelOne’s `InProcessClient32.dll`, suggesting a third-party hook is involved.

## Repro Environment
- Host: Windows 11 (x86 emulator) with SentinelOne Agent `24.2.3.471`.
- Build: MSVC Debug configuration (`build/msvc/Debug/railc_msvc.exe` with fresh `railc_msvc.pdb`).
- Debugger: `cdb.exe` (Windows 10 SDK 10.0.26100.6584) invoked via WSL path.

## Instrumentation Scripts
| Script | Purpose | Output |
| ------ | ------- | ------ |
| `cdb_cmds_capture.txt` | Baseline breakpoint on `PerformSetupAndTransfer`. | `cdb_capture.log` |
| `cdb_cmds_vtable.txt` | Dump slot `0x80` target per invocation. | `cdb_vtable.log` |
| `cdb_cmds_slot80.txt` | Record thunk target and call stack. | `cdb_slot80.log` |
| `cdb_cmds_tmwin.txt` | Trace `TMainWindow::SetupWindow` entry/return. | `cdb_tmwin.log` |
| `cdb_cmds_esi_trace.txt` | Single-step virtual call and log SentinelOne hook registers. | `cdb_esi_trace.log` |

## Key Observations
1. **Stable caller state**  
   Each time `owl::TWindow::PerformSetupAndTransfer` runs, ECX and ESI both point to valid `TWindow` instances within the process heap. The first log line from `cdb_esi_trace.log` shows this clearly:  
   ```
   [PerformSetup] entry this=01460b40 esi=01460b40 ecx=01460b40
   ```

2. **Virtual call target resolution**  
   Slot `0x80` resolves to the import thunk `0x00c6d08a`, which simply jumps to `TMainWindow::SetupWindow (0x00cb0650)`. Symbol disassembly confirms the thunk contains a single unconditional `jmp`.

3. **SentinelOne hook interposition**  
   Breakpoints inside `InProcessClient32+0x91161` and `+0x912f6` fire immediately after `CreateWindowExA` returns but before OWL regains control. The first hook breakpoint already shows the non-volatile register overwritten:  
   ```
   [Sentinel] hook exit esi=74e01560 ecx=74e01560
   ```
   The address `0x74e01560` lies inside the SentinelOne DLL’s image range, not within our executable or OWL’s heap.

4. **Crash site**  
   After the hook returns, `PerformSetupAndTransfer` executes `or dword ptr [esi+70h], 20h`. With ESI pointing into `InProcessClient32.dll`, the write lands on read-only memory, throwing `0xC0000005`. The registers captured immediately before the fault match this scenario and remain consistent across runs.

5. **Control sample**  
   Earlier child windows (status bar, toolbar, etc.) do not trigger the hook, and their `esi` register remains stable through the entire `PerformSetupAndTransfer` call. This isolates the issue to the SentinelOne instrumentation path taken during creation of the main frame window.

## Conclusion
SentinelOne’s `InProcessClient32.dll` post-`CreateWindowExA` hook violates the Windows x86 calling convention by clobbering ESI, a non-volatile register. When OWL resumes execution, it writes through the corrupted pointer and crashes. The application binaries and associated modernization changes are not at fault; disabling the SentinelOne hook (or running on a machine without the agent) should allow the program to launch successfully.

## Recommended Next Actions
1. **Disable or bypass SentinelOne** on the debugging host (temporary policy exemption or alternate VM). Re-run the Debug build to confirm the crash disappears.
2. **Collect supporting telemetry** while SentinelOne is active, if required:
   - Add a guard near `PerformSetupAndTransfer` to verify `esi` resides within our module/heap ranges and log when it does not.
   - Capture the call stack inside the Sentinel hook using `!analyze` or manual symbol loading if security policies require evidence.
3. **Once runtime stabilizes** (with SentinelOne disabled or corrected), execute the manual smoke checklist and re-validate both MSVC configurations.
