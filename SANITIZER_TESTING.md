# AddressSanitizer (Memory Leak Detection) Testing Guide

## Overview
This project includes AddressSanitizer support for detecting memory leaks, buffer overflows, and use-after-free bugs.

## Prerequisites
Ensure the following development packages are installed:
```bash
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgtk-3-dev pkg-config
```

## Building with AddressSanitizer

### Compile with sanitizers enabled:
```bash
make clean
make SANITIZE=1
```

This builds the executable with:
- `-fsanitize=address,undefined` - Address Sanitizer and Undefined Behavior Sanitizer
- `-fno-omit-frame-pointer` - Better stack traces in reports

## Running with Memory Checking

### Basic execution:
```bash
./ap
```

AddressSanitizer will automatically detect issues and display a report at runtime.

### Advanced execution with detailed output:
```bash
ASAN_OPTIONS=verbosity=2:log_path=asan.log ./ap
```

## What AddressSanitizer Detects

1. **Heap buffer overflows** - Writing past allocated memory
2. **Use-after-free** - Accessing memory after it's been freed
3. **Double-free** - Freeing the same memory twice
4. **Memory leaks** - Allocated but never freed memory
5. **Undefined behavior** - Uninitialized variables, integer overflows, etc.

## Known Memory Management Areas to Test

### PlayAudio Pipeline
- ✅ GStreamer element creation and cleanup
- ✅ Destructor properly unreferences pipeline and elements
- ✅ Dynamic pad linking (on_pad_added callback)

### Folder Navigation
- ✅ GDir handle properly closed after use
- ✅ String allocations freed correctly
- ✅ GTK widget data lifecycle

### GUI Updates
- ✅ GTK label and button updates
- ✅ Object data attachment/retrieval
- ✅ Thread-safe UI operations

## Sample Test Scenarios

### Test 1: Load and Play Multiple Files
```bash
./ap
# Click "Open Folder"
# Select a folder with audio files
# Play multiple files in sequence
# Check terminal for any ASAN errors
```

### Test 2: Rapid Play/Pause Toggle
```bash
./ap
# Load a file
# Rapidly click play/pause 10+ times
# Verify no use-after-free errors
```

### Test 3: Close During Playback
```bash
./ap
# Start playing a song
# Close the window while playing
# Check for proper cleanup
```

### Test 4: Invalid File Handling
```bash
./ap
# Try to open corrupted/invalid audio file
# Verify graceful error handling without crashes
```

## Interpreting AddressSanitizer Output

### Heap-use-after-free example:
```
ERROR: AddressSanitizer: heap-use-after-free on unknown address 0x60b0000a47a0
READ of size 4 at 0x60b0000a47a0 thread T0
    #0 0x55a7c8... in function_name /path/to/file.cpp:123:45
```

### Memory leak example:
```
Direct leak of 1024 byte(s) in 1 object(s) allocated from:
    #0 0x55a7c8... in malloc (/path/to/file.cpp:45:10)
    #1 0x55a7d9... in PlayAudio::PlayAudio() (/path/to/PlayAudio.cpp:50:20)
```

## Build Output Log
See `sanitizer_build.log` for full compilation details.

## Further Testing
- Run under valgrind for additional memory profiling
- Use GDB debugger for step-through testing
- Profile with `gprof` to identify performance bottlenecks
