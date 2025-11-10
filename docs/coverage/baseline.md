# Coverage Baseline

This file records the initial coverage baseline for `src/railcore` measured with OpenCppCoverage.

- How to reproduce locally:
  - Install OpenCppCoverage (e.g., `choco install opencppcoverage`).
  - Run `build.bat Debug COVERAGE=1`.
  - Cobertura XML is generated at `build/msvc/Debug/coverage.xml`.

- Scope: counts only sources under `src/railcore`, excludes `tests`, `third_party`, and `src/railui`.

- First baseline (to be updated by CI or local runs):
  - Date: 2025-10-21
  - File: `build/msvc/Debug/coverage.xml`
  - Notes: GoogleTest coverage (`coverage_gtest.xml`) is produced when GTests are built and run; merging reports is optional and not required for the soft gate.




