# Unit Testing Documentation

## Overview
This project includes comprehensive unit tests for core functionality. Tests are organized by feature and can be run independently or all together.

## Test Files

### 1. test_volume.cpp
**Purpose**: Volume control functionality testing

**Tests Included**:
- `test_volume_conversion()` - Verify slider values (0-100) convert to GStreamer volume (0.0-1.0)
- `test_volume_edge_cases()` - Test boundary conditions (1%, 99%, etc.)

**Build & Run**:
```bash
g++ -std=c++11 -Wall -g tests/test_volume.cpp -o tests/build/test_volume
./tests/build/test_volume
```

### 2. test_file_handling.cpp
**Purpose**: File and folder management functionality testing

**Tests Included**:
- `test_audio_file_detection()` - Verify .mp3, .wav, .ogg files are correctly identified
- `test_file_path_parsing()` - Extract filename from full file paths
- `test_song_list_sorting()` - Alphabetical sorting of playlists

**Build & Run**:
```bash
g++ -std=c++11 -Wall -g tests/test_file_handling.cpp -o tests/build/test_file_handling
./tests/build/test_file_handling
```

### 3. test_playlist.cpp
**Purpose**: Playlist navigation functionality testing

**Tests Included**:
- `test_play_next()` - Navigate to next song, with wraparound handling
- `test_play_previous()` - Navigate to previous song with loop-to-end behavior

**Build & Run**:
```bash
g++ -std=c++11 -Wall -g tests/test_playlist.cpp -o tests/build/test_playlist
./tests/build/test_playlist
```

## Running All Tests

### Option 1: Using Test Runner Script
```bash
./run_tests.sh
```

This script:
- Compiles all test files
- Runs each test sequentially
- Reports pass/fail summary

### Option 2: Manual Compilation and Execution
```bash
cd tests
mkdir -p build

# Compile each test
g++ -std=c++11 -Wall -g test_volume.cpp -o build/test_volume
g++ -std=c++11 -Wall -g test_file_handling.cpp -o build/test_file_handling
g++ -std=c++11 -Wall -g test_playlist.cpp -o build/test_playlist

# Run tests
./build/test_volume
./build/test_file_handling
./build/test_playlist
```

## Test Results

All unit tests pass successfully:

```
========== Volume Control Unit Tests ==========
✓ test_volume_conversion passed
✓ test_volume_edge_cases passed

========== File Handling Unit Tests ==========
✓ test_audio_file_detection passed
✓ test_file_path_parsing passed
✓ test_song_list_sorting passed

========== Playlist Navigation Unit Tests ==========
✓ test_play_next passed
✓ test_play_previous passed
```

## Test Coverage

### Functionality Covered
- ✅ Volume slider conversion (0-100 → 0.0-1.0)
- ✅ Audio file format detection (.mp3, .wav, .ogg)
- ✅ File path parsing and extraction
- ✅ Playlist sorting and navigation
- ✅ Edge cases and boundary conditions

### Not Yet Covered (Integration Testing Required)
- GStreamer pipeline operations (requires GTK/GStreamer libraries)
- Pause/Play state transitions
- GUI widget interactions
- File I/O operations
- Error handling with actual file system

## Integration Testing

For testing features that require GStreamer or GTK+:

1. **Manual Testing**:
   - Build and run the main application: `make && ./ap`
   - Test play/pause, volume, seeking manually
   - Verify UI responds correctly to user actions

2. **Memory Testing**:
   - Build with sanitizers: `make SANITIZE=1`
   - See `SANITIZER_TESTING.md` for details

3. **Functional Testing**:
   - Load folders with various audio files
   - Test playlist navigation (next/previous)
   - Test edge cases (empty folders, invalid files)
   - Verify error dialogs appear for problematic scenarios

## Adding New Tests

To add new unit tests:

1. Create a new `.cpp` file in the `tests/` directory
2. Follow the existing test pattern:
   ```cpp
   void test_feature_name() {
       std::cout << "Running: test_feature_name\n";
       
       // Test implementation with assertions
       assert(condition == expected);
       
       std::cout << "✓ test_feature_name passed\n";
   }
   
   int main() {
       std::cout << "\n========== Feature Tests ==========\n\n";
       test_feature_name();
       std::cout << "\n========== All tests passed! ==========\n\n";
       return 0;
   }
   ```

3. Add compilation rule to `run_tests.sh`
4. Run tests to verify: `./run_tests.sh`

## Continuous Testing Recommendations

1. **Before Each Commit**:
   - Run all unit tests: `./run_tests.sh`
   - Verify no compiler warnings: `make clean && make`

2. **Before Release**:
   - Run unit tests
   - Run with AddressSanitizer: `make SANITIZE=1 && ./ap`
   - Manual functional testing on target platform

3. **Post-Deployment**:
   - Monitor for crashes/errors in production
   - Update tests based on reported bugs
