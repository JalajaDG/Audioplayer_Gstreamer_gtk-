
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
                                         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        // Get the selected folder
        char *folder_path;
        folder_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

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
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200); // Set the size of the window


 // Create a vertical box to divide the window into two parts
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10); // Vertical box with 10px spacing

 // Create the first partition (can be a label or any widget)
    GtkWidget *label = gtk_label_new("First Partition");
    
    // Associate the label with the window
    g_object_set_data(G_OBJECT(window), "folder_label", label);
      // Create a button with the label 
    GtkWidget *button = gtk_button_new_with_label("open folder");
        gtk_widget_set_size_request(button, 100, 30);


    // Connect the button's "clicked" signal to the callback function
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window);


  // Add the first partition and the button to the vbox (vertical box)
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0); // Add label to the top partition
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0); // Add button to the bottom partition

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

