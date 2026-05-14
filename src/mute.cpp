#include "mute.h"
#include "PlayAudio.h"

static bool is_muted = false;
static gdouble saved_volume = 1.0;

void on_mute_clicked(GtkWidget *button, gpointer user_data) noexcept {
    (void)user_data;
    PlayAudio& player = PlayAudio::getInstance();
    GstElement* volume_elem = player.getVolumeElement();
    if (!volume_elem) {
        g_print("Mute: volume element not found!\n");
        return;
    }

    if (!is_muted) {
        g_object_get(volume_elem, "volume", &saved_volume, NULL);
        g_print("Mute clicked → saving current volume: %.2f\n", saved_volume);

        g_object_set(volume_elem, "volume", 0.0, NULL);
        g_print("Volume set to 0.0 (muted)\n");
        gtk_button_set_image(GTK_BUTTON(button),
            gtk_image_new_from_icon_name("audio-volume-muted-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR));

        is_muted = true;
    } else {
        g_object_set(volume_elem, "volume", saved_volume, NULL);
        g_print("Unmute clicked → restoring saved volume: %.2f\n", saved_volume);

        gtk_button_set_image(GTK_BUTTON(button),
            gtk_image_new_from_icon_name("audio-volume-high-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR));

        is_muted = false;
    }
}
