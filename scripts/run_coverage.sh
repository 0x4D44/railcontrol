#!/bin/bash
# Script to build and run tests with coverage reporting
# Usage: ./scripts/run_coverage.sh

set -e  # Exit on error

echo "=== RailControl Coverage Report Generator ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build/coverage"
COVERAGE_DIR="${BUILD_DIR}/coverage"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "${REPO_ROOT}"

# Clean previous build
if [ -d "${BUILD_DIR}" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf "${BUILD_DIR}"
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo -e "${GREEN}Configuring CMake with coverage enabled...${NC}"
cmake ../.. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_COVERAGE=ON

echo -e "${GREEN}Building...${NC}"
cmake --build . -j$(nproc)

echo -e "${GREEN}Running tests...${NC}"
ctest --output-on-failure

echo -e "${GREEN}Generating coverage report...${NC}"
make coverage

# Display summary
echo
echo "=== Coverage Summary ==="
lcov --summary "${COVERAGE_DIR}/coverage_filtered.info" 2>&1 | grep -E "lines|functions|branches" || true

echo
echo -e "${GREEN}Coverage report generated successfully!${NC}"
echo -e "HTML report: ${YELLOW}file://${REPO_ROOT}/${BUILD_DIR}/coverage/html/index.html${NC}"
echo

# Optionally open in browser (Linux)
if command -v xdg-open &> /dev/null; then
    read -p "Open coverage report in browser? (y/N) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        xdg-open "${COVERAGE_DIR}/html/index.html"
    fi
fi

exit 0
