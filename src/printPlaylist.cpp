#include "printPlaylist.h"

#include "PlayAudio.h"

#include "openFolder.h" // For player_state::current_song_index
#include "pause.h"
#include "favourite.h"
#include <iostream>
using namespace std;


void on_row_activated(GtkWidget *widget, GtkListBoxRow *row, gpointer data) {
    (void)widget;
    PlaylistData *pdata = static_cast<PlaylistData *>(data);
    GtkWidget *window = pdata->window;
    GtkWidget *dialog = pdata->dialog;

    // Get the index of the activated row
    int index = gtk_list_box_row_get_index(row);
    cout << "Song index clicked: " << index << '\n';
    string selected_song;

    if (index >= 0 && index < static_cast<int>(player_state::song_list.size())) {
        selected_song = player_state::song_list[index];
        cout << "Playing: " << selected_song << '\n';
        cout << "Folder path: " << player_state::folder_path << '\n';
        string Filepath = player_state::folder_path + "/" + selected_song;

        PlayAudio &player = PlayAudio::getInstance();
        player.play_audioFile(Filepath, player_state::folder_path, window);
    }

    // update global index
    player_state::current_song_index = index;

    // Update GTK label
    GtkWidget *song_label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
    if (song_label) {
        gtk_label_set_text(GTK_LABEL(song_label), selected_song.c_str());
        cout << "Now playing (from vector): " << selected_song << '\n';
    } else {
        cout << "⚠️ song_label not found in window!" << '\n';
    }

    // Close the dialog immediately
    gtk_widget_destroy(dialog);
}


// Function to show the audio files list
void ShowAudioFilesList(GtkWidget *widget, gpointer data) noexcept {
    (void)widget;
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *scrolled_window;
    GtkWidget *list_box;

    GtkWidget *window = GTK_WIDGET(data);

    // Create a dialog to display the audio files
    dialog = gtk_dialog_new_with_buttons("Audio Files",
                                         GTK_WINDOW(window),
                                         GTK_DIALOG_MODAL,
                                         "_Close", GTK_RESPONSE_CLOSE,
                                         NULL);

    // Get the content area of the dialog
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Add a scrolled window for the list box
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, 400, 300);
    gtk_container_add(GTK_CONTAINER(content_area), scrolled_window);

    // Create a GtkListBox to display the playlist
    list_box = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), list_box);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(list_box), TRUE); // Enable click activation

    // Pass both window and dialog via struct
    PlaylistData *pdata = new PlaylistData{window, dialog};

    // Connect the "row-activated" signal to the callback function
    g_signal_connect(list_box, "row-activated", G_CALLBACK(on_row_activated), pdata);

    // Add each audio file in the playlist to the list box
    for (const string &song : player_state::song_list) {
        GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        GtkWidget *label = gtk_label_new(song.c_str());
        gtk_box_pack_start(GTK_BOX(row_box), label, TRUE, TRUE, 0);

        // Use Favourites class to check if song is favourite
        Favourites *favourites = static_cast<Favourites*>(g_object_get_data(G_OBJECT(window), "favourites_ptr"));
        if (favourites && favourites->isFavourite(song)) {
            GtkWidget *fav_icon = gtk_image_new_from_icon_name("starred-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
            gtk_box_pack_start(GTK_BOX(row_box), fav_icon, FALSE, FALSE, 0);
        }

        gtk_list_box_insert(GTK_LIST_BOX(list_box), row_box, -1);
    }

    // Highlight currently playing song if available
    if (player_state::current_song_index >= 0 &&
        player_state::current_song_index < static_cast<int>(player_state::song_list.size())) {

        GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(list_box),
                                                           player_state::current_song_index);
        if (row) {
            gtk_list_box_select_row(GTK_LIST_BOX(list_box), row);
            gtk_widget_grab_focus(GTK_WIDGET(row));
        }
    }

    // Show all widgets in the dialog
    gtk_widget_show_all(dialog);

    // Run the dialog and wait for the user to close it
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog); // Destroy the dialog after use
}
