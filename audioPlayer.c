#include <gtk/gtk.h>

// Callback function when the window is closed
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // Exit the GTK main loop
}

// Callback function when the button is clicked
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Button clicked!\n"); // Print a message when the button is clicked
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
    
      // Create a button with the label 
    GtkWidget *button = gtk_button_new_with_label("open folder");
        gtk_widget_set_size_request(button, 100, 30);


    // Connect the button's "clicked" signal to the callback function
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);


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

