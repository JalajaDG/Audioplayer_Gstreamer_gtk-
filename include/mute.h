#pragma once

#include <gtk/gtk.h>
#include <gst/gst.h>

/**
 * @brief Handle mute button click
 * 
 * Callback function that toggles the mute state of the audio pipeline.
 * 
 * @param button The GTK button widget that was clicked
 * @param user_data User data (typically the main window)
 */
void on_mute_clicked(GtkWidget *button, gpointer user_data) noexcept;

