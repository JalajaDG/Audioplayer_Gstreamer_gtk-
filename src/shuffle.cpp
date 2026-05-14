#include "shuffle.h"
#include <iostream>

void on_shuffle_clicked(GtkWidget *button, gpointer user_data) noexcept {
    (void)user_data;
    player_state::shuffle_mode = !player_state::shuffle_mode;

    if (player_state::shuffle_mode) {
        std::cout << "Shuffle mode: ON\n";
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");
    } else {
        std::cout << "Shuffle mode: OFF\n";
        gtk_style_context_remove_class(gtk_widget_get_style_context(button), "suggested-action");
    }
}
