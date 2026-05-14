#pragma once

#include <gtk/gtk.h>

/**
 * @brief Toggle pause/resume playback state
 * 
 * Callback function for pause/play button click.
 * Toggles between PLAYING and PAUSED states in the GStreamer pipeline.
 * Handles edge cases like pipeline stuck in READY state.
 * 
 * @param widget The GTK button widget that was clicked
 * @param data User data (typically the main window)
 */
void toggle_pause(GtkWidget *widget, gpointer data) noexcept;

/**
 * @brief Update the pause button icon based on playback state
 * 
 * Changes the button icon to show current state:
 * - Play icon (▶) when paused
 * - Pause icon (⏸) when playing
 * 
 * @param pauseIcon The GTK button widget to update
 * @param isPlaying true if currently playing (show pause icon), false if paused (show play icon)
 */
void update_pause_icon(GtkWidget *pauseIcon, bool isPlaying) noexcept;

