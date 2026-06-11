#include "repeat.h"
#include <iostream>
#include <gtk/gtk.h>
using namespace std;

void on_repeat_clicked(GtkWidget *button, gpointer user_data) noexcept {
    (void)user_data;
    player_state::repeat_mode = !player_state::repeat_mode;

    if (player_state::repeat_mode) {
        cout << "Repeat mode: ON (Repeat One)\n";
        GtkWidget *image = gtk_image_new_from_icon_name(
            "media-playlist-repeat-song-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(button), image);
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");
    } else {
        cout << "Repeat mode: OFF\n";
        GtkWidget *image = gtk_image_new_from_icon_name(
            "media-playlist-repeat-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(button), image);
        gtk_style_context_remove_class(gtk_widget_get_style_context(button), "suggested-action");
    }
}
