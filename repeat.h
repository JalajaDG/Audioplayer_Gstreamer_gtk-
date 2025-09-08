#ifndef REPEAT_H
#define REPEAT_H

#include <gtk/gtk.h>

// Declare global flag (no memory allocated here)
extern bool repeat_mode;   // true = repeat current song, false = no repeat

void on_repeat_clicked(GtkWidget *button, gpointer user_data);

#endif
