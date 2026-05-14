#pragma once

#include <gtk/gtk.h>

/**
 * @brief Handle theme button click
 * 
 * Callback to toggle application theme.
 * 
 * @param button The GTK button widget that was clicked
 * @param user_data User data (typically the main window)
 */
void on_themeBtn_clicked(GtkWidget *button, gpointer user_data) noexcept;

