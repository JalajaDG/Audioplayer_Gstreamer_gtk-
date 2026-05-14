#include <gtk/gtk.h>
#include <gst/gst.h>

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
    GstElement *sink = (GstElement *)data;
    GstPad *sinkpad = gst_element_get_static_pad(sink, "sink");

    if (gst_pad_is_linked(sinkpad)) {
        gst_object_unref(sinkpad);
        return;
    }

    // Link the pad to the sink
    if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link pad to sink.\n");
    }

    gst_object_unref(sinkpad);
}

static void play_audio(GtkWidget *widget, gpointer data) {
    GstElement *pipeline = gst_pipeline_new("audio-player");
    GstElement *source = gst_element_factory_make("filesrc", "source");
    GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
    GstElement *sink = gst_element_factory_make("autoaudiosink", "sink");

    if (!pipeline || !source || !decoder || !sink) {
        g_printerr("Failed to create elements.\n");
        return;
    }

    g_object_set(source, "location", "/home/ee213082/Music/aud3.mp3", NULL);

    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);

    // Connect the decodebin's pad-added signal to link it with the sink
    g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), sink);

    // Link the source to the decoder
    if (gst_element_link(source, decoder) != TRUE) {
        g_printerr("Failed to link source to decoder.\n");
        return;
    }

    // Set the pipeline state to playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run GTK event loop
    gtk_main();

    gst_object_unref(pipeline);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Audio Player");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    GtkWidget *button = gtk_button_new_with_label("Play Audio");
    g_signal_connect(button, "clicked", G_CALLBACK(play_audio), NULL);

    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_widget_show_all(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_main();

    return 0;
}

