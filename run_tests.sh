#!/bin/bash
set -e

echo "=========================================="
echo "Audio Player Unit Test Runner"
echo "=========================================="
echo ""

TEST_SRC="tests/src"
BUILD_DIR="tests/build"
TESTS=()

mkdir -p "$BUILD_DIR"

echo "Compiling unit tests..."
echo ""

for test_src in "$TEST_SRC"/test_*.cpp; do
    name=$(basename "$test_src" .cpp)
    echo "Compiling $name..."
    g++ -std=c++17 -Wall -Wextra -g \
        -I include \
        "$test_src" \
        -o "$BUILD_DIR/$name"
    TESTS+=("$BUILD_DIR/$name")
done

echo ""
echo "=========================================="
echo "Running Tests"
echo "=========================================="
echo ""

PASSED=0
FAILED=0

for bin in "${TESTS[@]}"; do
    echo "Running: $(basename "$bin")"
    if "$bin"; then
        PASSED=$((PASSED + 1))
    else
        FAILED=$((FAILED + 1))
        echo "FAILED: $(basename "$bin")"
    fi
    echo ""
done

echo "=========================================="
echo "Test Results: $PASSED passed, $FAILED failed"
echo "=========================================="

if [ "$FAILED" -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi
