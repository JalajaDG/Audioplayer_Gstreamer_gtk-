#include "PlayAudio.h"
#include "openFolder.h"   // for player_state::current_song_index & player_state::folder_path
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <iostream>
#include <gst/pbutils/pbutils.h>

using namespace std;

void on_metadata_clicked(GtkWidget* widget, gpointer data) noexcept {
    (void)widget;
    GtkWidget* window = GTK_WIDGET(data);

    PlayAudio& player = PlayAudio::getInstance();
    GstElement* pipeline = player.getPipeline();
    if (!pipeline) return;

    if (player_state::song_list.empty() || player_state::current_song_index >= (int)player_state::song_list.size()) {
        g_printerr("No song selected\n");
        return;
    }

    string filepath = player_state::folder_path + "/" + player_state::song_list[player_state::current_song_index];
    g_print("Getting metadata for: %s\n", filepath.c_str());

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

    // Extract tags
    const GstTagList* tags = gst_discoverer_info_get_tags(info);
    gchar *title = nullptr, *artist = nullptr, *album = nullptr, *comment = nullptr, *year_str = nullptr;
    if (tags) {
        gst_tag_list_get_string(tags, GST_TAG_TITLE, &title);
        gst_tag_list_get_string(tags, GST_TAG_ARTIST, &artist);
        gst_tag_list_get_string(tags, GST_TAG_ALBUM, &album);
        gst_tag_list_get_string(tags, GST_TAG_COMMENT, &comment);
        gst_tag_list_get_string(tags, GST_TAG_DATE, &year_str);
    }

    // Duration in minutes and seconds
    GstClockTime duration = gst_discoverer_info_get_duration(info);
    guint total_sec = (guint)(duration / GST_SECOND);
    guint duration_min = total_sec / 60;
    guint duration_sec = total_sec % 60;

    // Container and audio info
    gchar *container = nullptr;
    gchar *codec = nullptr;
    guint channels = 0;
    guint sample_rate = 0;

    const GstDiscovererStreamInfo* sinfo_const = gst_discoverer_info_get_stream_info(info);
    if (sinfo_const) {
        GstDiscovererStreamInfo* sinfo = (GstDiscovererStreamInfo*)sinfo_const;
        GstCaps* caps = gst_discoverer_stream_info_get_caps(sinfo);

        if (caps && gst_caps_get_size(caps) > 0) {
            GstStructure* s = gst_caps_get_structure(caps, 0);
            container = g_strdup(gst_structure_get_name(s));

            gchar* caps_str = gst_caps_to_string(caps);
            if (g_str_has_prefix(caps_str, "audio/")) {
                codec = g_strdup(gst_structure_get_name(s));
                gint c = 0, rate = 0;
                gst_structure_get_int(s, "channels", &c);
                gst_structure_get_int(s, "rate", &rate);
                channels = c;
                sample_rate = rate;
            }
            g_free(caps_str);
        }
    }

    // Prepare formatted metadata with spacing and tabs
    gchar metadata_text[2048];
    snprintf(metadata_text, sizeof(metadata_text),
             "<b>General</b>\n"
             "  Title      : %s\n"
             "  Artist     : %s\n"
             "  Album      : %s\n"
             "  Year       : %s\n"
             "  Duration   : %u min %u sec\n"
             "  Comment    : %s\n"
             "  Container  : %s\n\n"
             "<b>Audio</b>\n"
             "  Codec      : %s\n"
             "  Channels   : %u\n"
             "  Sample rate: %u Hz",
             title ? title : "Unknown",
             artist ? artist : "Unknown",
             album ? album : "Unknown",
             year_str ? year_str : "Unknown",
             duration_min,
             duration_sec,
             comment ? comment : "Unknown",
             container ? container : "Unknown",
             codec ? codec : "Unknown",
             channels,
             sample_rate
    );

    // Create GTK dialog with markup
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                               static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               NULL);

    GtkWidget* label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), metadata_text);
    gtk_label_set_xalign(GTK_LABEL(label), 0); // left align

    GtkWidget* content_area = gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    // Cleanup
    g_free(title);
    g_free(artist);
    g_free(album);
    g_free(comment);
    g_free(year_str);
    g_free(container);
    g_free(codec);
    g_object_unref(info);
    g_object_unref(discoverer);
}
