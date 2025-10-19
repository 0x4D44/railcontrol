# Post-Crash Validation Checklist (to run after SentinelOne mitigation)

## 1. Runtime Smoke Tests
- [ ] Launch `build/msvc/Debug/railc_msvc.exe`; ensure main frame, status bar, toolbar, and child windows render correctly.
- [ ] Load each sample layout (`FAST.RCD`, `KINGSX.RCD`, `QUEENST.RCD`, `WAVERLY.RCD`) and verify no parsing errors.
- [ ] Trigger key dialogs (About, Configuration, Arrivals, Departures, Platform, Locoyard) and confirm interactions.
- [ ] Invoke Help (`F1`) and confirm WinHelp opens without errors.
- [ ] Repeat all steps with `build/msvc/Release/railc_msvc.exe`.

## 2. Logging & Diagnostics
- [ ] Capture a fresh `railc_runtime_smoke.log` summarizing the above actions (date/time stamped).
- [ ] Note any anomalies, even if non-blocking, in `docs/debug_journal_2025-10-17.md` or a new dated entry.

## 3. Automated Signals
- [ ] Run `build_msvc.bat Debug` and `build_msvc.bat Release`; ensure both finish warning-free.
- [ ] Trigger MSVC CI workflow (if applicable) and monitor for failures.
- [ ] Record CI run ID and status in `progress_summary_runtime.txt`.

## 4. Cleanup
- [ ] Remove or disable any temporary guard instrumentation added to work around SentinelOne.
- [ ] Archive the SentinelOne analysis artifacts in a shared location for IT reference.
- [ ] Update `progress_summary_runtime.txt` with final status and link to the smoke-test log.
