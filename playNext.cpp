#include "playNext.h"
#include "PlayAudio.h"
#include "openFolder.h"   // for folder_Path, song_list, currently_playing_song_index
#include <string>
#include <vector>

using namespace std;

// extern variables from openFolder.cpp
extern int currently_playing_song_index;
extern vector<string> song_list;
extern string folder_Path;

extern "C" void on_play_next_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);

    if (song_list.empty()) {
        g_print("Song list is empty. Nothing to play.\n");
        return;
    }

    // --- Move index ---
    if (currently_playing_song_index + 1 < (int)song_list.size()) {
        currently_playing_song_index++;
    } else {
        g_print("Reached end of playlist. Wrapping to first song.\n");
        currently_playing_song_index = 0;
    }

    // --- Build full file path ---
    string filepath = folder_Path + "/" + song_list[currently_playing_song_index];
    g_print("Next song selected: %s\n", filepath.c_str());

    // --- Play the song ---
    PlayAudio *player = PlayAudio::getInstance();
    player->play_audioFile(filepath, folder_Path, window);

    // --- Update label in UI ---
    GtkWidget *songLabel = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
    if (songLabel) {
        gtk_label_set_text(GTK_LABEL(songLabel), song_list[currently_playing_song_index].c_str());
    }
}
