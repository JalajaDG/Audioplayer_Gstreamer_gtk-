#pragma once

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <gst/gst.h>
#include "openFolder.h"

/**
 * @brief Data structure for playlist display dialog
 */
struct PlaylistData {
    GtkWidget* window;      ///< Main application window
    GtkWidget* dialog;      ///< Playlist dialog window
};

/**
 * @brief Display the list of audio files in a dialog
 * 
 * Shows all available songs from the current folder in a scrollable list.
 * 
 * @param widget The GTK button widget that was clicked
 * @param data User data (typically the main window)
 */
void ShowAudioFilesList(GtkWidget *widget, gpointer data) noexcept;

