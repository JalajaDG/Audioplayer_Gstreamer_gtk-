#include "volume.h"
#include "PlayAudio.h" // to access your volume_element singleton

void on_volume_slider_changed(GtkRange *range, gpointer user_data) {
    gdouble value = gtk_range_get_value(range); // 0-100
    gdouble gst_volume = value / 100.0;         // convert to 0.0 - 1.0 for GStreamer

    PlayAudio* player = PlayAudio::getInstance();
    if(player->getVolumeElement()) {
        g_object_set(G_OBJECT(player->getVolumeElement()), "volume", gst_volume, NULL);
    }
}


