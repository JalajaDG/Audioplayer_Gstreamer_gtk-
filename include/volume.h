#pragma once

#include <gtk/gtk.h>

/**
 * @brief Handle volume slider value changes
 * 
 * Callback function for volume slider widget.
 * Converts slider value (0-100) to GStreamer volume (0.0-1.0)
 * and updates the audio output volume.
 * 
 * @param range The GTK volume slider widget
 * @param user_data User data (typically unused)
 */
void on_volume_slider_changed(GtkRange *range, gpointer user_data) noexcept;

