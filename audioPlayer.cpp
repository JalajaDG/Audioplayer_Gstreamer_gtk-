
//g++ `pkg-config --cflags gtk+-3.0` audioPlayer.cpp -o ap `pkg-config --libs gtk+-3.0`

//g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`

#include<iostream>
#include <gtk/gtk.h>
#include<string.h>
#include <vector>
#include <gst/gst.h>


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

    // Set the URI to the selected song's file path
    g_object_set(playbin, "uri", g_strdup_printf("file://%s", file_path), NULL);

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


    // Create a FileChooserDialog for selecting folders
    dialog = gtk_file_chooser_dialog_new("Open Foldls /usr/include/gstreamer-1.0/gster",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);
                                   


     // Add a filter for audio files
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "audio/mpeg");  // .mp3
    gtk_file_filter_add_mime_type(filter, "audio/wav");   // .wav
    gtk_file_filter_add_mime_type(filter, "audio/ogg");   // .ogg
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        // Get the selected folder
        char *file_path;
        file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        // Extract the folder path from the full file path

        folder_path = g_path_get_dirname(file_path);
        // Print the folder path
        g_print("Selected folder: %s\n", folder_path);

        // Update the label with the selected folder path
        GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));

        gtk_label_set_text(GTK_LABEL(label), folder_path);

       
        
        
      

        // Open the directory and list audio files
        GDir *dir = g_dir_open(folder_path, 0, NULL);
        if (dir != NULL) {
            const gchar *file_name;
            while ((file_name = g_dir_read_name(dir)) != NULL) {
                if (g_str_has_suffix(file_name, ".mp3") ||
                    g_str_has_suffix(file_name, ".wav") ||
                    g_str_has_suffix(file_name, ".ogg")) {
                    song_list.push_back(file_name);
                }
            }
            g_dir_close(dir);
        }
        
     
        // If there are songs in the playlist, play the first one
        if (!song_list.empty()) {
            string full_path = string(folder_path) + "/" + song_list[0];
            play_selected_song(full_path.c_str());
        }

        // Printing out the playlist
        for (const auto &song : song_list) {
            g_print("Song in playlist: %s\n", song.c_str());
        }

        g_free(file_path);
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

