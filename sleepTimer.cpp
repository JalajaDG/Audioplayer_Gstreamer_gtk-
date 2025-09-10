#include "sleepTimer.h"
#include "PlayAudio.h"
#include <gst/gst.h>
#include <gtk/gtk.h>

// Keep track of the currently running timeout and last selected timer
static guint current_timer_id = 0;
static const char* last_selected_time = nullptr;

// Called when timer finishes
static gboolean on_timer_expired(gpointer user_data) {
    GtkWidget *button = GTK_WIDGET(user_data);

    g_print("Sleep timer expired! Stopping playback and exiting...\n");

    // Stop playback using PlayAudio singleton
    GstElement* pipeline = PlayAudio::getInstance()->getPipeline();
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        g_print("Playback stopped.\n");
    }

    // Reset main button style
    if (button)
        gtk_style_context_remove_class(gtk_widget_get_style_context(button), "suggested-action");

    current_timer_id = 0;
    last_selected_time = nullptr;

    gtk_main_quit();
    return G_SOURCE_REMOVE;
}

// Called when a sleep time is chosen
static void on_timer_selected(GtkWidget *menuitem, gpointer user_data) {
    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem)))
        return; // Only act when item becomes active

    const char *time = (const char*)user_data;
    GtkWidget *button = GTK_WIDGET(g_object_get_data(G_OBJECT(menuitem), "parent_button"));

    // Cancel previous timer if any
    if (current_timer_id > 0) {
        g_source_remove(current_timer_id);
        current_timer_id = 0;
        g_print("Previous sleep timer canceled.\n");
    }

    if (g_strcmp0(time, "Clear Timer") == 0) {
        // Reset button style
        if (button)
            gtk_style_context_remove_class(gtk_widget_get_style_context(button), "suggested-action");
        last_selected_time = nullptr;

        // Unset all radio items
        GSList *group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));
        for (GSList *iter = group; iter != nullptr; iter = iter->next)
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(iter->data), FALSE);

        return;
    }

    g_print("Sleep timer set for %s\n", time);
    last_selected_time = time;

    // Make main button green
    if (button)
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");

    int minutes = 0;
    if      (g_strcmp0(time, "1 min")  == 0) minutes = 1;
    else if (g_strcmp0(time, "2 min")  == 0) minutes = 2;
    else if (g_strcmp0(time, "10 min") == 0) minutes = 10;
    else if (g_strcmp0(time, "15 min") == 0) minutes = 15;
    else if (g_strcmp0(time, "30 min") == 0) minutes = 30;

    if (minutes > 0)
        current_timer_id = g_timeout_add_seconds(minutes * 60, on_timer_expired, button);
}

// Called when the sleep timer button is clicked
void on_sleep_timer_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget *menu = gtk_menu_new();

    // Create radio menu items, all initially inactive
    GSList *group = nullptr;
    GtkWidget *item_1  = gtk_radio_menu_item_new_with_label(group, "1 min"); group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item_1));
    GtkWidget *item_2  = gtk_radio_menu_item_new_with_label(group, "2 min"); group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item_2));
    GtkWidget *item_10 = gtk_radio_menu_item_new_with_label(group, "10 min"); group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item_10));
    GtkWidget *item_15 = gtk_radio_menu_item_new_with_label(group, "15 min"); group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item_15));
    GtkWidget *item_30 = gtk_radio_menu_item_new_with_label(group, "30 min"); group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item_30));

    // Clear Timer option
    GtkWidget *item_clear = gtk_menu_item_new_with_label("Clear Timer");

    // Store main button reference
    g_object_set_data(G_OBJECT(item_1),  "parent_button", button);
    g_object_set_data(G_OBJECT(item_2),  "parent_button", button);
    g_object_set_data(G_OBJECT(item_10), "parent_button", button);
    g_object_set_data(G_OBJECT(item_15), "parent_button", button);
    g_object_set_data(G_OBJECT(item_30), "parent_button", button);
    g_object_set_data(G_OBJECT(item_clear), "parent_button", button);

    // Append items
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_1);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_2);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_15);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_30);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_clear);

    // Connect signals
        g_signal_connect(item_clear, "activate", G_CALLBACK(on_timer_selected), (gpointer)"Clear Timer");

    g_signal_connect(item_1,  "toggled", G_CALLBACK(on_timer_selected), (gpointer)"1 min");
    g_signal_connect(item_2,  "toggled", G_CALLBACK(on_timer_selected), (gpointer)"2 min");
    g_signal_connect(item_10, "toggled", G_CALLBACK(on_timer_selected), (gpointer)"10 min");
    g_signal_connect(item_15, "toggled", G_CALLBACK(on_timer_selected), (gpointer)"15 min");
    g_signal_connect(item_30, "toggled", G_CALLBACK(on_timer_selected), (gpointer)"30 min");
   // g_signal_connect(item_clear, "activate", G_CALLBACK(on_timer_selected), (gpointer)"Clear Timer");

    // Highlight last selected timer (dot)
    if (last_selected_time) {
        if      (g_strcmp0(last_selected_time, "1 min")  == 0) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item_1),  TRUE);
        else if (g_strcmp0(last_selected_time, "2 min")  == 0) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item_2),  TRUE);
        else if (g_strcmp0(last_selected_time, "10 min") == 0) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item_10), TRUE);
        else if (g_strcmp0(last_selected_time, "15 min") == 0) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item_15), TRUE);
        else if (g_strcmp0(last_selected_time, "30 min") == 0) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item_30), TRUE);
    }

    gtk_widget_show_all(menu);

    gtk_menu_popup_at_widget(GTK_MENU(menu), button,
                             GDK_GRAVITY_SOUTH_WEST,
                             GDK_GRAVITY_NORTH_WEST,
                             NULL);
}
