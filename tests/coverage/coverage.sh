#!/usr/bin/env bash
#
# Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
#
# Builds the project in Test mode with gcov instrumentation, runs the test
# suite, and generates an HTML line-coverage report under
# tests/coverage/reports/html/.
#
# Requires: lcov, genhtml (Debian/Ubuntu: apt install lcov)

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build-coverage"
REPORTS_DIR="${PROJECT_ROOT}/tests/coverage/reports"
COVERAGE_INFO="${REPORTS_DIR}/coverage.info"
COVERAGE_REPORT_DIR="${REPORTS_DIR}/html"

cd "${PROJECT_ROOT}"
mkdir -p "${REPORTS_DIR}"

if command -v nproc > /dev/null; then
    NPROC=$(nproc)
elif command -v sysctl > /dev/null; then
    NPROC=$(sysctl -n hw.ncpu)
else
    NPROC=4
fi

for tool in lcov genhtml; do
    if ! command -v "${tool}" > /dev/null; then
        echo "ERROR: '${tool}' not found on PATH." >&2
        echo "Install it first:" >&2
        echo "  Debian/Ubuntu: sudo apt install lcov" >&2
        echo "  macOS:         brew install lcov" >&2
        exit 1
    fi
done

cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Test
cmake --build "${BUILD_DIR}" -j"${NPROC}"

ctest --test-dir "${BUILD_DIR}" --output-on-failure

lcov --directory "${BUILD_DIR}" --capture --ignore-errors inconsistent --output-file "${COVERAGE_INFO}"
lcov --remove "${COVERAGE_INFO}" \
    '/usr/*' \
    '*/vcpkg_installed/*' \
    '*/lib/*' \
    '*/tests/*' \
    '*/tests/*Tests.cpp' \
    --ignore-errors inconsistent,unused \
    --output-file "${COVERAGE_INFO}"

genhtml "${COVERAGE_INFO}" --output-directory "${COVERAGE_REPORT_DIR}"

echo "Coverage report generated at ${COVERAGE_REPORT_DIR}/index.html"