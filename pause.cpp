#include "pause.h"
#include "PlayAudio.h"

void toggle_pause(GtkWidget *widget, gpointer data) {
    std::cout << "Inside pause.cpp\n";

    PlayAudio* player = PlayAudio::getInstance();  // Get singleton instance
    if (!player) {
        std::cerr << "Error: PlayAudio instance not found!" << std::endl;
        return;
    }

    GstElement *pipeline = player->getPipeline();  // Retrieve the pipeline
    if (!pipeline) {
        std::cerr << "Error: Pipeline is null!" << std::endl;
        return;
    }

    GstState state;
    gst_element_get_state(pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
    std::cout << "Current Pipeline State: " << state << std::endl;

    // Ensure it's in PLAYING state before pausing
    if (state == GST_STATE_READY) {
        std::cout << "Pipeline stuck in READY. Trying to start playback first..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        gst_element_get_state(pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
        std::cout << "State after trying to play: " << state << std::endl;
    }

    // Proceed to pause if the pipeline is playing
    if (state == GST_STATE_PLAYING) {
        std::cout << "Pausing audio..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
        update_pause_icon(widget, false);  // show ▶️

    } 
    else if (state == GST_STATE_PAUSED) {
        std::cout << "Resuming audio..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        update_pause_icon(widget, true);   // show ⏸️
    } else {
        std::cerr << "Pipeline is not in a valid state to pause or play!" << std::endl;
    }

    // Verify state after attempting to change it
    gst_element_get_state(pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
    std::cout << "State after request: " << state << std::endl;
}

// Change button icon based on playing state
void update_pause_icon(GtkWidget *pauseIcon, bool isPlaying) {
    if (!pauseIcon) return;

    GtkWidget *image = gtk_image_new_from_icon_name(
        isPlaying ? "media-playback-pause" : "media-playback-start",
        GTK_ICON_SIZE_BUTTON
    );

    gtk_button_set_image(GTK_BUTTON(pauseIcon), image);
}