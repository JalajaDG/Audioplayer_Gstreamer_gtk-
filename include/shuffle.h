#pragma once

#include <gtk/gtk.h>
#include "player_state.h"

/**
 * @brief Handle shuffle button click
 * @param button The GTK button widget that was clicked
 * @param user_data User data (typically the main window)
 */
void on_shuffle_clicked(GtkWidget *button, gpointer user_data) noexcept;
