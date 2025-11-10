# Engine Scheduling – Next Steps (Stage-Based Transitions)

## Goals
- Evolve from fixed arrival/depart thresholds to a simple stage-based progression that can be validated in headless tests and consumed by the UI for richer status.
- Preserve current external behavior (arrived/departed flags, event ordering, deltas) while adding non-breaking, optional fields.

## Scope (Incremental)
1) Internal state for per-assignment progression (no public type changes yet).
2) Compose deltas with additional, optional fields (e.g., `stage`, `progressMs`) to convey intermediate state.
3) Tests for multi-tick/multi-entry ordering, non-reemit semantics, and reassign/reset.

## Design Outline
- Engine maintains an internal map: `assignmentState_[ttId] = {startClock, stageIndex, stageElapsed}`.
- Stages (initial slice):
  - 0 = assigned
  - 1 = arrived (>=200ms since start)
  - 2 = departed (>=500ms since start and after arrived)
  - UI note: Arrivals/Departures panes now append stage bucket/section telemetry when provided; they fall back to legacy status text when data is absent.
- On `AssignLoco`: reset state to stage=0, elapsed=0; clear arrived/departed flags; emit `LocoAssigned` event.
- On `ReleaseLoco`: remove internal state; emit `LocoReleased` event; delta clears `assignedLocoId`.
- On `Advance(dt)`: compute elapsed for each assignment; promote stages when thresholds crossed; queue DomainEvents and append per-entry `EntityDelta` fields:
  - `arrived=true` when crossing stage 1 (single emission)
  - `departed=true` when crossing stage 2 (single emission)
  - Optional: `stage` (0/1/2) and `progressMs` (monotonic, clamped)
- No auto-release: assignment persists after depart until explicit `ReleaseLoco` (as enforced by tests).
- Reassign resets timing and stage.
- Event ordering: arrival precedes departure within same tick when both thresholds crossed.

## API/Compatibility
- No changes to public headers. Optional delta fields (`stage`, `progressMs`) are additive in `EntityDelta.changedFields` for timetable entries.
- UI may ignore unknown changed fields and continue to rely on `arrived`/`departed`.

## Test Plan (GoogleTest)
- Progression across ticks: verify stage advances 0→1→2 with correct deltas and events.
- Same-tick both thresholds: assert both deltas present and arrival event precedes departure.
- Interleaving across multiple entries: ordering and independence maintained.
- Reassign after arrival: resets stage and re-emits arrival after threshold.
- Non-reemit of flags and stability under zero-dt ticks.

## Implementation Steps
1) Introduce `assignmentState_` in `engine_stub.cpp` next to existing maps; populate on assign/release.
2) In `Advance`, compute `elapsed = newClock - startClock`, derive stage index, and emit deltas/events accordingly (maintain existing `arrived_`/`departed_` sets for idempotence).
3) Optionally include `stage` and `progressMs` keys in `pendingTimetableExtraDeltas_` for UI richness.
4) Extend/adjust gtests to validate new optional fields behind feature guards if needed; keep existing tests green.

## Risks & Mitigations
- Risk: unintended behavior changes. Mitigation: keep new fields optional, preserve existing event/delta semantics and tests.
- Risk: performance on large layouts. Mitigation: O(nAssignments) per tick preserved; no heavy data structures.
