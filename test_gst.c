#include <gtk/gtk.h>
#include <gst/gst.h>

static void play_audio(GtkWidget *widget, gpointer data) {
    GstElement *pipeline = gst_pipeline_new("audio-player");
    GstElement *source = gst_element_factory_make("filesrc", "source");
    GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
    GstElement *sink = gst_element_factory_make("autoaudiosink", "sink");

    if (!pipeline || !source || !decoder || !sink) {
        g_printerr("Failed to create elements.\n");
        return;
    }

    g_object_set(source, "location", "/path/to/your/audio/file.mp3", NULL);

    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
    gst_element_link(source, decoder);
    gst_element_link(decoder, sink);

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

