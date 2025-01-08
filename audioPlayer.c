
//gcc `pkg-config --cflags gtk+-3.0` audioPlayer.c -o ap `pkg-config --libs gtk+-3.0`


#include <gtk/gtk.h>

// Callback function when the window is closed
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // Exit the GTK main loop
}

// Callback function when the button is clicked
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWidget *window = GTK_WIDGET(data);

    // Create a FileChooserDialog for selecting folders
    dialog = gtk_file_chooser_dialog_new("Open Folder",
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

        char *folder_path = g_path_get_dirname(file_path);
        // Print the folder path
        g_print("Selected folder: %s\n", folder_path);

        // Update the label with the selected folder path
        GtkWidget *label = g_object_get_data(G_OBJECT(window), "folder_label");
        gtk_label_set_text(GTK_LABEL(label), folder_path);

        g_free(folder_path); // Free the memory allocated for the folder path
    }

    gtk_widget_destroy(dialog); // Destroy the dialog after use
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

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

    
    GtkWidget *button = gtk_button_new_with_label("Open Folder"); // Button to open folder
 //   gtk_widget_set_size_request(button, 10, 10);
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window); // Connect button signal

    // Set the width of folderBox to 300 pixels (for example)
	//gtk_widget_set_size_request(folderBox, 10, 10); // 300px width, -1 for automatic height

    // Create the second partition (empty for now)
    GtkWidget *second_partition = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
   GtkWidget *second_label = gtk_label_new("Second Partition");
    gtk_box_pack_start(GTK_BOX(second_partition), second_label, TRUE, TRUE, 0); // Add the label to the second partition

   // Create THE 3RD PARTITON
   GtkWidget *features_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
   GtkWidget *third_label = gtk_label_new("features Partition");
   gtk_box_pack_start(GTK_BOX(features_box), third_label, TRUE, TRUE, 0); // Add the label to the second partition

   
   // Add label and button to the first partition
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

