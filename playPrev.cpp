#include "playPrev.h"
#include "PlayAudio.h"
#include "openFolder.h"   // for folder_Path, song_list, currently_playing_song_index
#include "shuffle.h"      // <-- include this to access shuffle_mode
#include "repeat.h"
#include <string>
#include <vector>

using namespace std;

// Function called when the "play previous" button is clicked
void on_play_prev_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);

    if (song_list.empty()) {
        g_print("Song list is empty. Nothing to play.\n");
        return;
    }

    int next_index = currently_playing_song_index; // default: current song

    if (repeat_mode) {
        // Repeat current song
        g_print("Repeat mode ON → Replaying current song index: %d\n", currently_playing_song_index);
        // next_index stays the same
    } 
    else if (shuffle_mode) {
        // Shuffle previous
        if (song_list.size() > 1) {
            int rand_index;
            do {
                rand_index = rand() % song_list.size();
            } while (rand_index == currently_playing_song_index); // avoid same song
            next_index = rand_index;
        }
        g_print("Shuffle mode ON → Random previous song index: %d\n", next_index);
    } 
    else {
        // Sequential previous
        if (currently_playing_song_index > 0) {
            next_index--;
        } else {
            g_print("Reached beginning of playlist. Wrapping to last song.\n");
            next_index = song_list.size() - 1;
        }
    }

    // Update currently playing index
    currently_playing_song_index = next_index;

    // Build full file path
    string filepath = folder_Path + "/" + song_list[currently_playing_song_index];
    g_print("Previous song selected: %s\n", filepath.c_str());

    // Play the song
    PlayAudio *player = PlayAudio::getInstance();
    player->play_audioFile(filepath, folder_Path, window);

    // Update label in UI
    GtkWidget *songLabel = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
    if (songLabel) {
        gtk_label_set_text(GTK_LABEL(songLabel), song_list[currently_playing_song_index].c_str());
    }
}
