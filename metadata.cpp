#include "PlayAudio.h"
#include "openFolder.h"   // for currently_playing_song_index & folder_Path
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <iostream>
#include <gst/pbutils/pbutils.h>   // <-- for GstDiscoverer


using namespace std;

// Callback for "Metadata" button click
void on_metadata_clicked(GtkWidget* widget, gpointer data) {
    GtkWidget* window = GTK_WIDGET(data);

    // Get the singleton player
    PlayAudio* player = PlayAudio::getInstance();
    if (!player) {
        g_printerr("Error: PlayAudio instance is NULL\n");
        return;
    }

    // Ensure pipeline exists
    GstElement* pipeline = player->getPipeline();
    if (!pipeline) {
        g_printerr("Error: pipeline is NULL\n");
        return;
    }

    // Get current song filepath
    if (song_list.empty() || currently_playing_song_index >= (int)song_list.size()) {
        g_printerr("No song is currently selected\n");
        return;
    }

    string filepath = folder_Path + "/" + song_list[currently_playing_song_index];
    g_print("Getting metadata for: %s\n", filepath.c_str());

    // Create GstDiscoverer
    GError* err = nullptr;
    GstDiscoverer* discoverer = gst_discoverer_new(5 * GST_SECOND, &err);
    if (!discoverer) {
        g_printerr("Failed to create GstDiscoverer: %s\n", err->message);
        g_error_free(err);
        return;
    }

    GstDiscovererInfo* info = gst_discoverer_discover_uri(discoverer, ("file://" + filepath).c_str(), &err);
    if (!info) {
        g_printerr("Failed to discover URI: %s\n", err->message);
        g_error_free(err);
        g_object_unref(discoverer);
        return;
    }

    const GstTagList* tags = gst_discoverer_info_get_tags(info);
    gchar* title = nullptr;
    gchar* artist = nullptr;
    gchar* album = nullptr;

    if (tags) {
        gst_tag_list_get_string(tags, GST_TAG_TITLE, &title);
        gst_tag_list_get_string(tags, GST_TAG_ARTIST, &artist);
        gst_tag_list_get_string(tags, GST_TAG_ALBUM, &album);
    }

    // Prepare metadata string
    gchar metadata_text[1024];
    snprintf(metadata_text, sizeof(metadata_text),
             "Title : %s\nArtist: %s\nAlbum : %s",
             title ? title : "Unknown",
             artist ? artist : "Unknown",
             album ? album : "Unknown");

    // Print to console (optional)
    g_print("%s\n", metadata_text);

    // Show GTK dialog
  GtkWidget* dialog = gtk_message_dialog_new(
    GTK_WINDOW(window),
    static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
    GTK_MESSAGE_INFO,
    GTK_BUTTONS_OK,
    "%s",
    metadata_text
);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    // Cleanup
    g_free(title);
    g_free(artist);
    g_free(album);
    g_object_unref(info);
    g_object_unref(discoverer);
}
