#ifndef SEEK_H
#define SEEK_H

#include <iostream>
#include <gtk/gtk.h>
#include <gst/gst.h>

using namespace std;

// Function to handle user changing the seek bar
void on_seek_changed(GtkRange *range, gpointer user_data);

// Function to periodically update the seek bar according to playback
gboolean update_seek_position(gpointer user_data);

#endif // SEEK_H
