#pragma once

#include <gtk/gtk.h>
#include <string_view>
#include "player_state.h"

/**
 * @brief Callback when "Open Folder" button is clicked
 * @param widget The GTK button widget that was clicked
 * @param data User data (typically the main window)
 */
void on_openFolder_clicked(GtkWidget *widget, gpointer data) noexcept;

/**
 * @brief Load default music folder on application startup
 * @param window GTK window widget for displaying messages
 */
void open_default_folder(GtkWidget *window) noexcept;
