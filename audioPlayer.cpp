
//g++ `pkg-config --cflags gtk+-3.0` audioPlayer.cpp -o ap `pkg-config --libs gtk+-3.0`

//g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`

#include<iostream>
#include <gtk/gtk.h>
#include<string.h>
#include <vector>
#include <gst/gst.h>

#include<algorithm> //for sort
using namespace std;
static char *folder_path;
static vector<string> song_list;
static GstElement *playbin = NULL; // Global GStreamer playbin element for playback





static void play_selected_song(const char *file_path) {
    // Check if the playbin element exists, if not, create it
    if (playbin == NULL) {
        playbin = gst_element_factory_make("playbin", "playbin");
        if (!playbin) {
            g_printerr("Failed to create playbin element\n");
            return;
        }
    }
      // Stop any currently playing song
    gst_element_set_state(playbin, GST_STATE_NULL);

   // Explicitly set the URI for the new file
    gchar *uri = g_strdup_printf("file://%s", file_path);
    g_object_set(playbin, "uri", uri, NULL);
    g_free(uri);

    // Set the playbin state to playing
    GstStateChangeReturn ret = gst_element_set_state(playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline to PLAYING state\n");
    } else {
        g_print("Now playing: %s\n", file_path);
    }
}


// Callback function when the window is closed
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // Exit the GTK main loop
}

// Callback function when the button is clicked
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWidget *window = GTK_WIDGET(data);

    int currently_playing_song_index = -1; // -1 means no song is currently playing.

    // Create a FileChooserDialog for selecting folders
    dialog = gtk_file_chooser_dialog_new(
        "Open Folder",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, // Select folders only
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        // Get the selected folder path
        char *folder_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_print("Selected folder: %s\n", folder_path);

        // Update the label with the selected folder path
        GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));
        if (label != NULL) {
            gtk_label_set_text(GTK_LABEL(label), folder_path);
        }

        // Open the directory and list audio files
        GDir *dir = g_dir_open(folder_path, 0, NULL);
        if (dir != NULL) {
            const gchar *file_name;
            song_list.clear(); // Clear the current playlist

            while ((file_name = g_dir_read_name(dir)) != NULL) {
                if (g_str_has_suffix(file_name, ".mp3") ||
                    g_str_has_suffix(file_name, ".wav") ||
                    g_str_has_suffix(file_name, ".ogg")) {
                    song_list.push_back(file_name); // Add valid audio files to the playlist
                }
            }
            
   	 // Sort the song_list alphabetically
   	 sort(song_list.begin(), song_list.end());

            g_dir_close(dir);
        } else {
            g_print("Failed to open the directory: %s\n", folder_path);
        }

        // Play the first song in the playlist, if available
        if (!song_list.empty()) {
            currently_playing_song_index = 0;
            std::string full_path = std::string(folder_path) + "/" + song_list[currently_playing_song_index];
            play_selected_song(full_path.c_str());
            g_print("Now playing: %s\n", full_path.c_str());
        } else {
            g_print("No audio files found in the selected folder.\n");
        }

        // Print the playlist for debugging
        g_print("Song list:\n");
        for (size_t i = 0; i < song_list.size(); ++i) {
            g_print("  [%zu] %s\n", i, song_list[i].c_str());
        }

        g_free(folder_path); // Free dynamically allocated memory
    }

    gtk_widget_destroy(dialog); // Destroy the dialog after use
}

// Function to show the audio files list
static void ShowAudioFilesList(GtkWidget *widget, gpointer data) {
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

    // Add each audio file in the playlist to the list box
    for (const string &song : song_list) {
        GtkWidget *label = gtk_label_new(song.c_str());
        gtk_list_box_insert(GTK_LIST_BOX(list_box), label, -1);
    }

    // Show all widgets in the dialog
    gtk_widget_show_all(dialog);

    // Run the dialog and wait for the user to close it
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog); // Destroy the dialog after use
    }

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);
//initialize gstreamer
    gst_init(&argc, &argv);

    // Create a window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL); // Create a top-level window
    gtk_window_set_title(GTK_WINDOW(window), "Simple GTK Window"); // Set the title of the window
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400); // Set the size of the window


 // Create a vertical box to divide the window into two parts
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); // Vertical box with 10px spacing
    
    //FOLDER BOX
    GtkWidget *folderBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget *label = gtk_label_new("No folder selected"); // Label to display the folder path
    g_object_set_data(G_OBJECT(window), "folder_label", label); // Store the label in the window's data

    
    GtkWidget *button =     gtk_button_new_from_icon_name("folder", GTK_ICON_SIZE_SMALL_TOOLBAR);
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window); // Connect button signal

    
    // Create the second partition (empty for now)
    GtkWidget *second_partition = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
   GtkWidget *second_label = gtk_label_new("Second Partition");
    gtk_box_pack_start(GTK_BOX(second_partition), second_label, TRUE, TRUE, 0); // Add the label to the second partition

   // Create THE 3RD PARTITON
   GtkWidget *features_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
GtkWidget *ShowAudioFilesIcon = gtk_button_new_from_icon_name("multimedia-volume-control", GTK_ICON_SIZE_SMALL_TOOLBAR);
//gtk_widget_set_size_request(ShowAudioFilesIcon, 50, 50);  // Set width and height to 50px

        // Store the song_list in the window data=bcz we need to pass this song_list vector to another function
        g_object_set_data(G_OBJECT(window), "song_list", &song_list);
        g_print("Song list set with %zu songs\n", song_list.size());


       g_signal_connect(ShowAudioFilesIcon, "clicked", G_CALLBACK(ShowAudioFilesList), window); // Connect the click signal
 




    // Add icon  to the features box
    gtk_box_pack_start(GTK_BOX(features_box), ShowAudioFilesIcon, FALSE, FALSE, 0);  // Expand label

   
   // Add label and button to the folder box
    gtk_box_pack_start(GTK_BOX(folderBox), label, TRUE, TRUE, 0);  // Expand label
    gtk_box_pack_start(GTK_BOX(folderBox), button, FALSE, FALSE, 0); // Button takes minimal space


  // Add the  folderbox,featurebox and second box  to the vbox (vertical box)
    gtk_box_pack_start(GTK_BOX(vbox), folderBox, FALSE, FALSE, 0); // Add label to the top partition
    gtk_box_pack_start(GTK_BOX(vbox), second_partition, TRUE, TRUE, 0); // Add empty second partition
    gtk_box_pack_start(GTK_BOX(vbox), features_box, FALSE, FALSE, 0); // Add button to the bottom partition

    // Add the vbox to the window
    gtk_container_add(GTK_CONTAINER(window), vbox);


    // Connect the window destroy signal to exit GTK when the window is closed
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Show the window
    gtk_widget_show_all(window);

    // Start the GTK main event loop
    gtk_main();

    return 0;
}

