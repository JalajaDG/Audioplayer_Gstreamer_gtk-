#include "player_state.h"

namespace player_state {
    std::vector<std::string> song_list;
    std::string folder_path;
    int current_song_index = 0;
    bool shuffle_mode = false;
    bool repeat_mode = false;
    std::mutex state_mutex;
}
