#include "repeat.h"
#include <iostream>
#include <gtk/gtk.h>

// Define global variable (memory allocated here)
bool repeat_mode = false;   // default OFF

void on_repeat_clicked(GtkWidget *button, gpointer user_data) {
    repeat_mode = !repeat_mode;  // toggle ON/OFF

    if (repeat_mode) {
        std::cout << "Repeat mode: ON (Repeat One)\n";
        GtkWidget *image = gtk_image_new_from_icon_name(
            "media-playlist-repeat-song-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(button), image);
    } else {
        std::cout << "Repeat mode: OFF\n";
        GtkWidget *image = gtk_image_new_from_icon_name(
            "media-playlist-repeat-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(button), image);
    }
}
