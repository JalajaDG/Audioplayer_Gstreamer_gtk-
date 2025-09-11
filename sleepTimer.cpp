#include "sleepTimer.h"
#include "PlayAudio.h"
#include <gst/gst.h>
#include <gtk/gtk.h>

static guint current_timer_id = 0;
static const char* last_selected_time = nullptr;
static GtkWidget* last_selected_item = NULL;

// Called when timer finishes
static gboolean on_timer_expired(gpointer user_data) {
    GtkWidget *button = GTK_WIDGET(user_data);

    g_print("Sleep timer expired! Stopping playback and exiting...\n");
    GstElement* pipeline = PlayAudio::getInstance()->getPipeline();
    if (pipeline) gst_element_set_state(pipeline, GST_STATE_NULL);

    if (button)
        gtk_style_context_remove_class(gtk_widget_get_style_context(button), "suggested-action");

    if (last_selected_item)
        gtk_widget_override_background_color(last_selected_item, GTK_STATE_FLAG_NORMAL, NULL);

    current_timer_id = 0;
    last_selected_time = NULL;
    last_selected_item = NULL;

    gtk_main_quit();
    return G_SOURCE_REMOVE;
}

// Called when a sleep time is chosen
static void on_timer_selected(GtkWidget *menuitem, gpointer user_data) {
    const char *time = (const char*)user_data;
    GtkWidget *button = GTK_WIDGET(g_object_get_data(G_OBJECT(menuitem), "parent_button"));

    // Cancel previous timer
    if (current_timer_id > 0) {
        g_source_remove(current_timer_id);
        current_timer_id = 0;
    }

    // Remove highlight from previous item
    if (last_selected_item)
        gtk_widget_override_background_color(last_selected_item, GTK_STATE_FLAG_NORMAL, NULL);

    if (g_strcmp0(time, "Clear Timer") == 0) {
        if (button)
            gtk_style_context_remove_class(gtk_widget_get_style_context(button), "suggested-action");
        last_selected_time = NULL;
        last_selected_item = NULL;
        return;
    }

    // Highlight main button
    if (button)
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");

    // Highlight selected menu item (bright background)
    GdkRGBA color;
    gdk_rgba_parse(&color, "lightgreen");
    gtk_widget_override_background_color(menuitem, GTK_STATE_FLAG_NORMAL, &color);

    last_selected_item = menuitem;
    last_selected_time = time;

    // Set timeout
    int minutes = 0;
    if      (g_strcmp0(time, "1 min")  == 0) minutes = 1;
    else if (g_strcmp0(time, "2 min")  == 0) minutes = 2;
    else if (g_strcmp0(time, "10 min") == 0) minutes = 10;
    else if (g_strcmp0(time, "15 min") == 0) minutes = 15;
    else if (g_strcmp0(time, "30 min") == 0) minutes = 30;

    if (minutes > 0)
        current_timer_id = g_timeout_add_seconds(minutes * 60, on_timer_expired, button);

    g_print("Sleep timer set for %s\n", time);
}

// Called when sleep timer button is clicked
void on_sleep_timer_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget *menu = gtk_menu_new();

    const char* options[] = { "1 min", "2 min", "10 min", "15 min", "30 min", "Clear Timer" };
    for (int i = 0; i < 6; i++) {
        GtkWidget *item = gtk_menu_item_new_with_label(options[i]);
        g_object_set_data(G_OBJECT(item), "parent_button", button);
        g_signal_connect(item, "activate", G_CALLBACK(on_timer_selected), (gpointer)options[i]);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    }

    gtk_widget_show_all(menu);
    gtk_menu_popup_at_widget(GTK_MENU(menu), button,
                             GDK_GRAVITY_SOUTH_WEST,
                             GDK_GRAVITY_NORTH_WEST,
                             NULL);
}
