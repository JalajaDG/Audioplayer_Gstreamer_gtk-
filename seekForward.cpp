#include "seekForward.h"
#include "PlayAudio.h"
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <iostream>

using namespace std;

void on_skip_forward_clicked(GtkWidget *widget, gpointer data) {
  
    GtkWidget *window = GTK_WIDGET(data);  // window passed from g_signal_connect
    PlayAudio *player = PlayAudio::getInstance();
    GstElement *pipeline = player->getPipeline();

    if (!player || !pipeline) {
        cout << "⚠️ No active player or pipeline!" << endl;
        return;
    }

    gint64 current = 0, duration = 0;
    if (!gst_element_query_position(pipeline, GST_FORMAT_TIME, &current)) {
        cout << "⚠️ Could not query current position!" << endl;
        return;
    }

    if (!gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration)) {
        cout << "⚠️ Could not query duration!" << endl;
        return;
    }

    // Add 10 seconds (in nanoseconds)
    gint64 new_position = current + (10 * GST_SECOND);

    // Don’t seek beyond total duration
    if (new_position > duration) {
        new_position = duration;
    }

    cout << "⏩ Skipping forward 10s: "
         << (current / GST_SECOND) << "s → "
         << (new_position / GST_SECOND) << "s" << endl;

    // Perform seek
    gst_element_seek_simple(
        pipeline,
        GST_FORMAT_TIME,
        (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
        new_position
    );

    // ✅ Update the current time label immediately
    GtkWidget *current_time_label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "current_time_label"));
    if (current_time_label) {
        int cur_sec = (new_position / GST_SECOND) % 60;
        int cur_min = (new_position / GST_SECOND) / 60;
        char cur_text[16];
        snprintf(cur_text, sizeof(cur_text), "%02d:%02d", cur_min, cur_sec);
        gtk_label_set_text(GTK_LABEL(current_time_label), cur_text);
    }
}
