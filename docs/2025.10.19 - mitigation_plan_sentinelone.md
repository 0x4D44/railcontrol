# SentinelOne Mitigation Plan (2025-10-17)

## Goal
Restore the ability to launch and debug `railc_msvc.exe` by neutralizing the SentinelOne hook that corrupts the `ESI` register during window creation, while capturing any evidence IT/security need to approve an exemption.

## Experiment Order
1. **Baseline confirmation (5 min)**
   - Action: Launch `railc_msvc.exe` under CDB with SentinelOne active to reconfirm failure signature and gather the latest hook trace (`cdb_esi_trace.log`).
   - Success criteria: Reproduce `ESI -> InProcessClient32.dll` corruption; logs ready for IT handoff.

2. **Temporary agent disablement (primary path)**
   - Owner: Security/IT.
   - Action: Request a time-boxed SentinelOne policy exemption for the developer workstation or move the project to a clean VM without the agent.
   - Verification: Re-run the MSVC Debug build under CDB; confirm `PerformSetupAndTransfer` succeeds and the main window paints.

3. **Guarded build (fallback path)**
   - Trigger: If SentinelOne cannot be disabled promptly.
   - Action: Prototype a defensive check inside `owl::TWindow::PerformSetupAndTransfer` (local fork) that validates `this/esi` against known module heaps. On mismatch, log diagnostic data and bail before the write.
   - Risk: This is a temporary shim; avoid committing to source control. Guard must be compiled only in diagnostic builds.
   - Verification: Run under SentinelOne to ensure the guard detects corruption without crashing; capture logs for security escalation.

4. **Evidence package for IT (parallel)**
   - Prepare concise summary (pull from `docs/debug_analysis_sentinelone.md`) plus relevant log excerpts demonstrating the register clobber.
   - Deliverable: Single ZIP containing `cdb_esi_trace.log`, `debug_analysis_sentinelone.md`, and screenshots or stack traces as required.

5. **Post-mitigation validation**
   - After SentinelOne is out of the path (either exemption or guard), proceed to the smoke-test checklist (see `docs/2025.10.17 - manual_smoke_test_checklist.md`) for both Debug and Release builds.
   - Additionally, trigger the MSVC CI pipeline to ensure no regressions appeared during the mitigation.

## Dependencies & Timing
- **IT coordination** is the critical path; guard code is only a fallback to keep debugging moving while waiting for approval.
- Ensure all diagnostic binaries and scripts remain under version control only as documentation; do not commit the guard change unless the team agrees on an official workaround.
