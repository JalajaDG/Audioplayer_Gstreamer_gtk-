#pragma once

#include <gtk/gtk.h>

/**
 * @brief Handle sleep timer button click
 * 
 * Callback to configure and activate the sleep timer.
 * 
 * @param button The GTK button widget that was clicked
 * @param user_data User data (typically the main window)
 */
void on_sleep_timer_clicked(GtkWidget *button, gpointer user_data) noexcept;

