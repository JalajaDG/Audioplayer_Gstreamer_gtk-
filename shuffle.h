#ifndef SHUFFLE_H
#define SHUFFLE_H

#include <gtk/gtk.h>

extern bool shuffle_mode;   // true = shuffle ON, false = OFF
void on_shuffle_clicked(GtkWidget *button, gpointer user_data);

#endif
