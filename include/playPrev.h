#pragma once

#include <gtk/gtk.h>

/**
 * @brief Handle play previous button click
 * 
 * Callback to go back to the previous song in the playlist.
 * 
 * @param widget The GTK button widget that was clicked
 * @param data User data (typically the main window)
 */
void on_play_prev_clicked(GtkWidget *widget, gpointer data) noexcept;

