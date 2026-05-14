#pragma once

#include <gtk/gtk.h>

/**
 * @brief Callback when metadata button is clicked
 * 
 * Displays metadata for the currently playing song.
 * 
 * @param button The GTK button widget that was clicked
 * @param user_data User data (typically the main window)
 */
void on_metadata_clicked(GtkWidget *button, gpointer user_data) noexcept;

