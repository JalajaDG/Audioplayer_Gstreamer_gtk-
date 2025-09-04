#include "openFolder.h"
#include "PlayAudio.h"

vector<string> song_list; // Define the global song list
string folder_Path;  // ✅ Define the global variable so that u can acces folder path ,when new song pressed from printplaylist

static GstElement *playbin = NULL;



static void play_selected_item(string full_path,string folder_path) {
	
   // cout<<"inside play_slected_file:-passed folder_path value"<<folder_path<<endl;
// Convert file_path and folder_path to string
//check if selected item is song/video
    if (g_str_has_suffix(full_path.c_str(), ".mp3") ||
        g_str_has_suffix(full_path.c_str(), ".wav") ||
        g_str_has_suffix(full_path.c_str(), ".ogg"))
     {
     
         PlayAudio *player = PlayAudio::getInstance();   // ✅ use singleton
        g_print("Playing first song: %s\n", full_path.c_str());
        player->openFolderAndPlayFirstSong(full_path);

                    	
                    	
      }
  
}





// Callback function when the button is clicked
 void on_openFolder_clicked(GtkWidget *widget, gpointer data) {
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
        cout<<"inside on_openFolder_clicked->folderpath="<<folder_path<<endl;
        // Play the first song in the playlist, if available
        if (!song_list.empty()) {
            currently_playing_song_index = 0;
          string full_path = string(folder_path) + "/" + song_list[currently_playing_song_index];
          //  play_selected_item(full_path.c_str());
          
         //  string folder_Path=string(folder_path);
           folder_Path = string(folder_path);  // ✅ Use global variable

            // Create a C++ thread to play the selected item..bcz if not the next lines (printing vetor and closing file explorer doesn't happen)
        //    / Create a C++ thread to play the selected item
            std::thread play_thread([full_path]() { //no need to pass folder_path as its global inside openFolder.h and all files can use it
                play_selected_item(full_path, folder_Path); // Pass by reference
            });

           

                        play_thread.detach(); // Detach the thread so it runs independently


            g_print("Now playing: %s\n", full_path.c_str());
        } else {
            g_print("No audio files found in the selected folder.\n");
        }

        // Print the playlist for debugging
        g_print("Song list:\n");
        for (size_t i = 0; i < song_list.size(); ++i) {
            g_print("  [%zu] %s\n", i, song_list[i].c_str());
        }

       // g_free(folder_path); // Free dynamically allocated memory
    }

    gtk_widget_destroy(dialog); // Destroy the dialog after use
}
