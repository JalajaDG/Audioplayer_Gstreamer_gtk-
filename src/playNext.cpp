#include "playNext.h"
#include "PlayAudio.h"
#include "openFolder.h"
#include "shuffle.h"
#include "repeat.h"
#include <string>
#include <mutex>

void on_play_next_clicked(GtkWidget *widget, gpointer data) noexcept {
    (void)widget;
    GtkWidget *window = GTK_WIDGET(data);

    std::lock_guard<std::mutex> lock(player_state::state_mutex);

    if (player_state::song_list.empty()) {
        g_print("Song list is empty. Nothing to play.\n");
        return;
    }

    int next_index = player_state::current_song_index;

    if (player_state::repeat_mode) {
        g_print("Repeat mode ON -> Replaying current song index: %d\n", player_state::current_song_index);
    } else if (player_state::shuffle_mode) {
        if (player_state::song_list.size() > 1) {
            int rand_index;
            do {
                rand_index = rand() % static_cast<int>(player_state::song_list.size());
            } while (rand_index == player_state::current_song_index);
            next_index = rand_index;
        }
        g_print("Shuffle mode ON -> Random next song index: %d\n", next_index);
    } else {
        if (player_state::current_song_index + 1 < static_cast<int>(player_state::song_list.size())) {
            next_index++;
        } else {
            g_print("Reached end of playlist. Wrapping to first song.\n");
            next_index = 0;
        }
    }

    player_state::current_song_index = next_index;
    std::string filepath = player_state::folder_path + "/" + player_state::song_list[next_index];
    std::string folder   = player_state::folder_path;

    // Release lock before calling into GStreamer/GTK
    // (the lock_guard destructs at end of scope, but we need the values already)
    g_print("Next song: %s\n", filepath.c_str());

    PlayAudio::getInstance().play_audioFile(filepath, folder, window);

    GtkWidget *songLabel = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
    if (songLabel)
        gtk_label_set_text(GTK_LABEL(songLabel), player_state::song_list[next_index].c_str());
}
