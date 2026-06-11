#pragma once

#include <vector>
#include <string>
#include <mutex>
using namespace std;

/// Central store for all shared player state.
/// Any access from outside the GTK main thread must hold state_mutex.
namespace player_state {
    extern vector<string> song_list;
    extern string folder_path;
    extern int current_song_index;
    extern bool shuffle_mode;
    extern bool repeat_mode;
    extern mutex state_mutex;
}
