#include "shuffle.h"
#include <iostream>

bool shuffle_mode = false;

void on_shuffle_clicked(GtkWidget *button, gpointer user_data) {
    shuffle_mode = !shuffle_mode;

   if (shuffle_mode) {
    std::cout << "Shuffle mode: ON\n";
    gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");
} else {
    std::cout << "Shuffle mode: OFF\n";
    gtk_style_context_remove_class(gtk_widget_get_style_context(button), "suggested-action");
}

}
