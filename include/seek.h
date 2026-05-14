#pragma once

#include <gtk/gtk.h>
#include <gst/gst.h>

/**
 * @brief Handle user changing the seek bar
 * 
 * Callback for seek bar position changes during playback.
 * 
 * @param range The GTK range (seek bar) widget
 * @param user_data User data pointer
 */
void on_seek_changed(GtkRange *range, gpointer user_data) noexcept;

/**
 * @brief Periodically update the seek bar according to playback
 * 
 * Timer callback that updates the seek bar position to reflect
 * current playback position.
 * 
 * @param user_data User data pointer
 * @return TRUE to continue calling, FALSE to stop
 */
gboolean update_seek_position(gpointer user_data) noexcept;

