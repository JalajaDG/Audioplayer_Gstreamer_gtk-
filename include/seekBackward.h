#pragma once

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <gst/gst.h>
#include "openFolder.h"

/**
 * @brief Handle skip backward button click
 * 
 * Skips backward in the currently playing song by a fixed duration.
 * 
 * @param widget The GTK button widget that was clicked (skip backward button)
 * @param data User data (typically the main window)
 */
void on_skip_backward_clicked(GtkWidget *widget, gpointer data) noexcept;



