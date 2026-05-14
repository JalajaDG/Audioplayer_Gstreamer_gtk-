#include "theme.h"

// Callback to toggle dark/light theme
void on_themeBtn_clicked(GtkWidget *button, gpointer user_data) noexcept {
    (void)user_data;

    // Get current dark theme setting
    gboolean dark;
    g_object_get(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", &dark, NULL);

    // Toggle theme
    dark = !dark;
    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", dark, NULL);

    // Switch button icon to indicate next mode
    const char *icon_name = dark ? "display-brightness-symbolic"  // dark mode → show sun for light
                                 : "night-light-symbolic";       // light mode → show moon for dark

    GtkWidget *icon = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button), icon);

    // Update tooltip
    gtk_widget_set_tooltip_text(button, dark ? "Switch to Light Mode" : "Switch to Dark Mode");
}
