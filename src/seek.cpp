#include "seek.h"
#include "PlayAudio.h"
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <iostream>

void on_seek_changed(GtkRange *range, gpointer data) noexcept {
    (void)data;
    PlayAudio& player = PlayAudio::getInstance();
    GstElement *pipeline = player.getPipeline();
    if (!pipeline) return;

    GstState state;
    gst_element_get_state(pipeline, &state, NULL, 0);
    if (state != GST_STATE_PLAYING && state != GST_STATE_PAUSED) return;

    gdouble value = gtk_range_get_value(range);

    gint64 duration = 0;
    if (!gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration)) return;

    gint64 seek_pos = (gint64)(value / 100.0 * duration);

    gst_element_seek(pipeline,
                     1.0,
                     GST_FORMAT_TIME,
                     (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                     GST_SEEK_TYPE_SET, seek_pos,
                     GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

gboolean update_seek_position(gpointer data) noexcept {
    GtkWidget *window = GTK_WIDGET(data);

    GtkWidget *seekBox       = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "seek_box"));
    GtkWidget *current_label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "current_time_label"));
    GtkWidget *total_label   = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "total_time_label"));

    PlayAudio& player = PlayAudio::getInstance();
    GstElement *pipeline = player.getPipeline();
    if (!pipeline) return TRUE;

    gint64 current = 0, duration = 0;
    if (!gst_element_query_position(pipeline, GST_FORMAT_TIME, &current)) return TRUE;
    if (!gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration)) return TRUE;
    if (duration == 0) return TRUE;

    gdouble percent = (gdouble)current / duration * 100.0;

    g_signal_handlers_block_by_func(seekBox, (gpointer)on_seek_changed, window);
    gtk_range_set_value(GTK_RANGE(seekBox), percent);
    g_signal_handlers_unblock_by_func(seekBox, (gpointer)on_seek_changed, window);

    int cur_sec = (current / GST_SECOND) % 60;
    int cur_min = (current / GST_SECOND) / 60;
    char cur_text[16];
    snprintf(cur_text, sizeof(cur_text), "%02d:%02d", cur_min, cur_sec);
    gtk_label_set_text(GTK_LABEL(current_label), cur_text);

    int dur_sec = (duration / GST_SECOND) % 60;
    int dur_min = (duration / GST_SECOND) / 60;
    char dur_text[16];
    snprintf(dur_text, sizeof(dur_text), "%02d:%02d", dur_min, dur_sec);
    gtk_label_set_text(GTK_LABEL(total_label), dur_text);

    return TRUE;
}
