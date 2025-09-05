
//g++ `pkg-config --cflags gtk+-3.0` audioPlayer.cpp -o ap `pkg-config --libs gtk+-3.0`

//g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`

// g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp openFolder.cpp   -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`

// g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp openFolder.cpp  printPlaylist.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`
// g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp openFolder.cpp  printPlaylist.cpp PlayAudio.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`
// g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp openFolder.cpp  printPlaylist.cpp PlayAudio.cpp pause.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0` 
// g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp openFolder.cpp  printPlaylist.cpp PlayAudio.cpp pause.cpp seek.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`
// g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp openFolder.cpp  printPlaylist.cpp PlayAudio.cpp pause.cpp seek.cpp  seekForward.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`
// g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp openFolder.cpp  printPlaylist.cpp PlayAudio.cpp pause.cpp seek.cpp  seekForward.cpp seekBackward.cpp-o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`


#include<iostream>
#include <gtk/gtk.h>
#include<string.h>
#include <vector>
#include <gst/gst.h>

//seprated files
#include "pause.h"

#include "openFolder.h" 
#include "printPlaylist.h"
#include "PlayAudio.h"
#include "seek.h"
#include "seekForward.h"
#include "seekBackward.h"
#include<algorithm> //for sort
using namespace std;
static char *folder_path;
static GstElement *playbin = NULL; // Global GStreamer playbin element for playback






// Callback function when the window is closed
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // Exit the GTK main loop
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
    g_signal_connect(button, "clicked", G_CALLBACK(on_openFolder_clicked), window); // Connect button signal

    
    // Create the second partition (empty for now)
    GtkWidget *second_partition = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
   GtkWidget *songLabel = gtk_label_new("No song playing yet");
   g_object_set_data(G_OBJECT(window), "songLabel", songLabel);

    gtk_box_pack_start(GTK_BOX(second_partition), songLabel, TRUE, TRUE, 0); // Add the label to the second partition
// --- Create a seek box (slider) ---
GtkWidget *seekBox = gtk_scale_new_with_range(
    GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0); // min=0, max=100, step=1
gtk_scale_set_draw_value(GTK_SCALE(seekBox), FALSE); // Don't show numeric value
gtk_widget_set_hexpand(seekBox, TRUE);               // Expand to fill width

// Store seekBox in window for later use
g_object_set_data(G_OBJECT(window), "seek_box", seekBox);




// --- Create current time and total duration labels ---
GtkWidget *current_time_label = gtk_label_new("00:00");
GtkWidget *total_time_label = gtk_label_new("00:00");
// Store labels in window for later use
g_object_set_data(G_OBJECT(window), "current_time_label", current_time_label);
g_object_set_data(G_OBJECT(window), "total_time_label", total_time_label);



//seek_box(conatins current time,seekbar,total time)
GtkWidget *seek_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
// Add labels and seekbox to the horizontal box
gtk_box_pack_start(GTK_BOX(seek_hbox), current_time_label, FALSE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(seek_hbox), seekBox, TRUE, TRUE, 5);
gtk_box_pack_start(GTK_BOX(seek_hbox), total_time_label, FALSE, FALSE, 5);




    //create playback parttion -play,pause,stop
    //GtkWidget *playback_partition

   // Create THE features PARTITON
   GtkWidget *features_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
GtkWidget *ShowAudioFilesIcon = gtk_button_new_from_icon_name("multimedia-volume-control", GTK_ICON_SIZE_SMALL_TOOLBAR);//in termianl =gtk3-icon-browser

//skip backward-10s
GtkWidget *skip_backward=gtk_button_new_from_icon_name("media-skip-backward",GTK_ICON_SIZE_SMALL_TOOLBAR);
// Store skip backward button in window for later use
g_object_set_data(G_OBJECT(window), "skip_backward", skip_backward);

GtkWidget *pauseIcon = gtk_button_new_from_icon_name("media-playback-pause", GTK_ICON_SIZE_SMALL_TOOLBAR);
// Store pause button in window for later use
g_object_set_data(G_OBJECT(window), "pause_button", pauseIcon);

   //skip forward-10s
    GtkWidget *skip_forward=gtk_button_new_from_icon_name("media-skip-forward",GTK_ICON_SIZE_SMALL_TOOLBAR); //gtk3-icon-browser
    // Store skip_forward button in window for later use
    g_object_set_data(G_OBJECT(window),"skip_forward",skip_forward);

//gtk_widget_set_size_request(ShowAudioFilesIcon, 50, 50);  // Set width and height to 50px

        // Store the song_list in the window data=bcz we need to pass this song_list vector to another function
        g_object_set_data(G_OBJECT(window), "song_list", &song_list);
        g_print("Song list set with %zu songs\n", song_list.size());


       g_signal_connect(ShowAudioFilesIcon, "clicked", G_CALLBACK(ShowAudioFilesList), window); // Connect the click signal
        g_signal_connect(pauseIcon, "clicked", G_CALLBACK(toggle_pause), window);
        g_signal_connect(seekBox, "value-changed", G_CALLBACK(on_seek_changed), window); //connect the seekbar
        g_signal_connect(skip_forward,"clicked",G_CALLBACK(on_skip_forward_clicked),window); //connect the skip_forward button with its function
        g_signal_connect(skip_backward,"clicked",G_CALLBACK(on_skip_backward_clicked),window); //connect the skip_backward button with its function





    // Add icon  to the features box
    gtk_box_pack_start(GTK_BOX(features_box), ShowAudioFilesIcon, FALSE, FALSE, 0);  // Expand label
    gtk_box_pack_start(GTK_BOX(features_box),skip_backward,FALSE,FALSE,0);
 gtk_box_pack_start(GTK_BOX(features_box), pauseIcon, FALSE, FALSE, 0);
 gtk_box_pack_start(GTK_BOX(features_box),skip_forward,FALSE,FALSE,0);
   
   // Add label and button to the folder box
    gtk_box_pack_start(GTK_BOX(folderBox), label, TRUE, TRUE, 0);  // Expand label
    gtk_box_pack_start(GTK_BOX(folderBox), button, FALSE, FALSE, 0); // Button takes minimal space


  // Add the  folderbox,featurebox and second box  to the vbox (vertical box)
    gtk_box_pack_start(GTK_BOX(vbox), folderBox, FALSE, FALSE, 0); // Add label to the top partition
    gtk_box_pack_start(GTK_BOX(vbox), second_partition, TRUE, TRUE, 0); // Add empty second partition
    gtk_box_pack_start(GTK_BOX(vbox), seek_hbox, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), features_box, FALSE, FALSE, 0); // Add button to the bottom partition

    // Add the vbox to the window
    gtk_container_add(GTK_CONTAINER(window), vbox);


    // Connect the window destroy signal to exit GTK when the window is closed
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Show the window
    gtk_widget_show_all(window);
    
    // Start updating seekbar and time labels every 0.5s
g_timeout_add(500, (GSourceFunc)update_seek_position, window);

    // Start the GTK main event loop
    gtk_main();

    return 0;
}

