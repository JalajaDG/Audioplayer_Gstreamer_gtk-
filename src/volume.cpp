#include "volume.h"
#include "PlayAudio.h"

void on_volume_slider_changed(GtkRange *range, gpointer user_data) noexcept {
    (void)user_data;
    gdouble value = gtk_range_get_value(range);
    gdouble gst_volume = value / 100.0;

    PlayAudio& player = PlayAudio::getInstance();
    if (player.getVolumeElement()) {
        g_object_set(G_OBJECT(player.getVolumeElement()), "volume", gst_volume, NULL);
    }
}
