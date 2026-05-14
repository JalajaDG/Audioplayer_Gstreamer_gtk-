cmake_minimum_required(VERSION 3.16)
project(AudioPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug or Release)" FORCE)
endif()

# ── Compiler flags ────────────────────────────────────────────────────────────
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g -O0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-O2 -DNDEBUG)
endif()

# ── Sanitizer support (cmake -DSANITIZE=ON) ───────────────────────────────────
option(SANITIZE "Enable AddressSanitizer and UBSan" OFF)
if(SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# ── Dependencies via pkg-config ───────────────────────────────────────────────
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3      REQUIRED gtk+-3.0)
pkg_check_modules(GST       REQUIRED gstreamer-1.0)
pkg_check_modules(GST_PBUTILS REQUIRED gstreamer-pbutils-1.0)

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    src/player_state.cpp
    src/audioPlayer.cpp
    src/openFolder.cpp
    src/printPlaylist.cpp
    src/PlayAudio.cpp
    src/pause.cpp
    src/seek.cpp
    src/seekForward.cpp
    src/seekBackward.cpp
    src/playNext.cpp
    src/playPrev.cpp
    src/repeat.cpp
    src/shuffle.cpp
    src/metadata.cpp
    src/theme.cpp
    src/sleepTimer.cpp
    src/favourite.cpp
    src/mute.cpp
    src/volume.cpp
)

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(ap ${SOURCES})

target_include_directories(ap PRIVATE
    include
    ${GTK3_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${GST_PBUTILS_INCLUDE_DIRS}
)

target_link_libraries(ap PRIVATE
    ${GTK3_LIBRARIES}
    ${GST_LIBRARIES}
    ${GST_PBUTILS_LIBRARIES}
    Threads::Threads
)

find_package(Threads REQUIRED)
target_link_libraries(ap PRIVATE Threads::Threads)

target_compile_options(ap PRIVATE
    ${GTK3_CFLAGS_OTHER}
    ${GST_CFLAGS_OTHER}
    ${GST_PBUTILS_CFLAGS_OTHER}
)

# ── Unit tests ────────────────────────────────────────────────────────────────
enable_testing()

foreach(TEST_NAME test_volume test_file_handling test_playlist)
    add_executable(${TEST_NAME} tests/src/${TEST_NAME}.cpp)
    target_include_directories(${TEST_NAME} PRIVATE include)
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -std=c++17)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# ── Install rules ─────────────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS ap RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY data/ DESTINATION ${CMAKE_INSTALL_DATADIR}/audioplayer)
install(DIRECTORY icons/ DESTINATION ${CMAKE_INSTALL_DATADIR}/audioplayer/icons)
