#ifndef PAUSE_H
#define PAUSE_H

//#include <iostream>
#include <gtk/gtk.h>
//#include <gst/gst.h>

void toggle_pause(GtkWidget *widget, gpointer data);

// Update button icon (true = playing, false = paused)
void update_pause_icon(GtkWidget *pauseIcon, bool isPlaying);

#endif  // PAUSE_H
