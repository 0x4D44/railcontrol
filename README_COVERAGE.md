# Code Coverage Guide

## Overview

This document explains how to generate and view code coverage reports for the RailControl project.

## Current Coverage Status

![Coverage](https://img.shields.io/badge/coverage-5%25-red)
![Target](https://img.shields.io/badge/target-98%25-green)

**Current Coverage:** ~5%
**Target Coverage:** 98%
**Gap:** 93 percentage points

See [Coverage Analysis Report](wrk_docs/2025.11.07%20-%20CC%20-%20Code%20Coverage%20Analysis%20Report.md) for detailed breakdown.

## Prerequisites

### Linux/WSL
```bash
sudo apt-get install cmake g++ lcov libgtest-dev
```

### macOS
```bash
brew install cmake lcov googletest
```

### Windows
- Install Visual Studio 2022 with C++ support
- Install CMake
- lcov not needed (use Visual Studio coverage tools)

## Quick Start

### Option 1: Using the Script (Recommended)
```bash
./scripts/run_coverage.sh
```

This script will:
1. Configure CMake with coverage enabled
2. Build the project
3. Run all tests
4. Generate HTML coverage report
5. Display coverage summary

### Option 2: Manual Steps
```bash
# Configure with coverage
cmake -B build/coverage \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_COVERAGE=ON

# Build
cmake --build build/coverage -j$(nproc)

# Run tests
cd build/coverage
ctest --output-on-failure

# Generate coverage report
make coverage

# View report
open coverage/html/index.html  # macOS
xdg-open coverage/html/index.html  # Linux
```

## Viewing Coverage Reports

### HTML Report
After running coverage, open:
```
build/coverage/coverage/html/index.html
```

This provides:
- Line-by-line coverage highlighting
- Function coverage percentages
- Branch coverage analysis
- File-by-file breakdown

### Command Line Summary
```bash
lcov --summary build/coverage/coverage/coverage_filtered.info
```

### IDE Integration

#### Visual Studio Code
1. Install "Coverage Gutters" extension
2. Run tests with coverage
3. Coverage will show in editor margins

#### Visual Studio 2022
1. Build with coverage: `Build > Analyze Code Coverage`
2. View results in Code Coverage window

## Coverage by Module

| Module | Files | Lines | Coverage | Status |
|--------|-------|-------|----------|--------|
| **RailCore** | 3 | ~750 | ~80% | ✅ Good |
| **RailUI** | 23 | ~12,450 | 0% | ❌ Critical |
| **Utilities** | 0 | 0 | N/A | ⏳ Planned |
| **Overall** | 26 | ~13,200 | ~5% | ❌ Critical |

## Improving Coverage

We are following a 7-stage plan to reach 98% coverage:

- **Stage 0** (Current): Infrastructure setup ✅
- **Stage 1**: Enhance RailCore tests (5% → 15%)
- **Stage 2**: Extract & test utilities (15% → 30%)
- **Stage 3**: Business logic tests (30% → 50%)
- **Stage 4**: Refactor for testability (50% → 70%)
- **Stage 5**: UI testing (70% → 85%)
- **Stage 6**: Comprehensive coverage (85% → 98%)

See [Implementation Plan](wrk_docs/2025.11.07%20-%20CC%20-%20Coverage%20Improvement%20Implementation%20Plan.md) for details.

## Writing Tests

### Test Structure
```cpp
// tests/railcore/example_test.cpp
#include <gtest/gtest.h>
#include "railcore/engine.h"

TEST(ModuleName, TestDescription) {
    // Arrange
    auto engine = CreateEngine(config, repo, nullptr, nullptr, nullptr, nullptr);

    // Act
    Status result = engine->SomeOperation();

    // Assert
    EXPECT_EQ(result.code, StatusCode::Ok);
}
```

### Running Specific Tests
```bash
cd build/coverage
./railcore_tests --gtest_filter=ModuleName.TestDescription
```

### Coverage-Driven Development
1. Write test first (TDD)
2. Run coverage to see uncovered lines
3. Add tests to cover missing branches
4. Verify coverage improved

## CI/CD Integration

Coverage runs automatically on:
- Every push to `main`, `develop`, or `claude/*` branches
- Every pull request

### Coverage Checks
- Minimum coverage threshold: 5% (will increase as we improve)
- Coverage reports uploaded to Codecov
- HTML reports available as build artifacts

### Local Pre-Commit Check
```bash
# Add to .git/hooks/pre-commit
./scripts/run_coverage.sh
if [ $? -ne 0 ]; then
    echo "Coverage check failed!"
    exit 1
fi
```

## Troubleshooting

### "lcov: command not found"
Install lcov: `sudo apt-get install lcov`

### "GTest not found"
Install GoogleTest: `sudo apt-get install libgtest-dev`

### Coverage shows 0%
- Ensure you built with `-DENABLE_COVERAGE=ON`
- Ensure tests ran successfully (`ctest` shows passes)
- Check that .gcda files exist in build directory

### Tests timeout
Increase timeout: `ctest --timeout 600`

## Coverage Metrics Explained

- **Line Coverage**: % of source lines executed
- **Function Coverage**: % of functions called
- **Branch Coverage**: % of conditional branches taken
- **Target**: We aim for 98% line coverage, 95% branch coverage

## Best Practices

1. **Write tests before code** (TDD)
2. **Test edge cases** (null, empty, max values)
3. **Test error paths** (invalid input, failures)
4. **Keep tests focused** (one concept per test)
5. **Use descriptive test names** (documents behavior)
6. **Don't test implementation details** (test behavior)
7. **Mock external dependencies** (files, network, UI)

## Resources

- [GoogleTest Documentation](https://google.github.io/googletest/)
- [lcov Documentation](http://ltp.sourceforge.net/coverage/lcov.php)
- [Coverage Analysis Report](wrk_docs/2025.11.07%20-%20CC%20-%20Code%20Coverage%20Analysis%20Report.md)
- [Implementation Plan](wrk_docs/2025.11.07%20-%20CC%20-%20Coverage%20Improvement%20Implementation%20Plan.md)

## Questions?

See the coverage improvement plan or ask the development team.

---

**Last Updated:** 2025-11-07
**Current Phase:** Stage 0 - Infrastructure Setup ✅
**Next Milestone:** Stage 1 - RailCore Enhancement (Target: 15% coverage)
