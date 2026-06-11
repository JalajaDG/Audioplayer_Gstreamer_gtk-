#include "player_state.h"
using namespace std;

namespace player_state {
    vector<string> song_list;
    string folder_path;
    int current_song_index = 0;
    bool shuffle_mode = false;
    bool repeat_mode = false;
    mutex state_mutex;
}
