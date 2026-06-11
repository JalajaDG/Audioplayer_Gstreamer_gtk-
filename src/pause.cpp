#include "pause.h"
#include "PlayAudio.h"
using namespace std;

void toggle_pause(GtkWidget *widget, gpointer data) noexcept {
    (void)data;
    cout << "Inside pause.cpp\n";

    PlayAudio& player = PlayAudio::getInstance();

    GstElement *pipeline = player.getPipeline();
    if (!pipeline) {
        cerr << "Error: Pipeline is null!" << endl;
        return;
    }

    GstState state;
    gst_element_get_state(pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
    cout << "Current Pipeline State: " << state << endl;

    if (state == GST_STATE_READY) {
        cout << "Pipeline stuck in READY. Trying to start playback first..." << endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        gst_element_get_state(pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
        cout << "State after trying to play: " << state << endl;
    }

    if (state == GST_STATE_PLAYING) {
        cout << "Pausing audio..." << endl;
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
        update_pause_icon(widget, false);
    } else if (state == GST_STATE_PAUSED) {
        cout << "Resuming audio..." << endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        update_pause_icon(widget, true);
    } else {
        cerr << "Pipeline is not in a valid state to pause or play!" << endl;
    }

    gst_element_get_state(pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
    cout << "State after request: " << state << endl;
}

void update_pause_icon(GtkWidget *pauseIcon, bool isPlaying) noexcept {
    if (!pauseIcon) return;

    GtkWidget *image = gtk_image_new_from_icon_name(
        isPlaying ? "media-playback-pause" : "media-playback-start",
        GTK_ICON_SIZE_BUTTON
    );

    gtk_button_set_image(GTK_BUTTON(pauseIcon), image);
}
