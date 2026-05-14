#pragma once

#include <gtk/gtk.h>

/**
 * @brief Handle play next button click
 * 
 * Callback to skip to the next song in the playlist.
 * 
 * @param widget The GTK button widget that was clicked
 * @param data User data (typically the main window)
 */
void on_play_next_clicked(GtkWidget *widget, gpointer data) noexcept;

