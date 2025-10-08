#include "openFolder.h" //for currently_playing_song_index & folder_path

#include "PlayAudio.h"
#include "pause.h"
#include "repeat.h"  
#include "shuffle.h"
#include "favourite.h"
#include "mute.h"
#include "volume.h"

int currently_playing_song_index;
PlayAudio* PlayAudio::instance = nullptr;  // Initialize singleton instance

// Singleton access method
PlayAudio* PlayAudio::getInstance() {
    if (instance == nullptr) {
        instance = new PlayAudio();
    }
    return instance;
}
 


PlayAudio::PlayAudio()
 : pipeline(nullptr), filesrc(nullptr), decodebin(nullptr),
   tee(nullptr),
   queue_audio(nullptr), audiobin(nullptr),
   queue_visualizer(nullptr), visualizer_bin(nullptr),
   videobin(nullptr), queue_video_sink(nullptr), videoconvert(nullptr), gtksink(nullptr),
   goom(nullptr), volume_element(nullptr), bus(nullptr), msg(nullptr)
{
    gst_init(nullptr, nullptr);

    pipeline = gst_pipeline_new("audio_pipeline");
    if (!pipeline) {
        g_printerr("Failed to create pipeline\n");
        return;
    }

    /* Create main elements */
    filesrc = gst_element_factory_make("filesrc", "filesrc");
    decodebin = gst_element_factory_make("decodebin", "decodebin");
    tee = gst_element_factory_make("tee", "audio_tee");

    queue_audio = gst_element_factory_make("queue", "queue_audio");
    queue_visualizer = gst_element_factory_make("queue", "queue_visualizer");

    if (!filesrc || !decodebin || !tee || !queue_audio || !queue_visualizer) {
        g_printerr("Failed to create main pipeline elements\n");
        return;
    }

    /* ------------------ audiobin ------------------ */
    audiobin = gst_bin_new("audiobin");
    GstElement *audio_convert = gst_element_factory_make("audioconvert", "audio_convert");
    GstElement *audio_resample = gst_element_factory_make("audioresample", "audio_resample");
    volume_element = gst_element_factory_make("volume", "volume");
    GstElement *autoaudiosink = gst_element_factory_make("autoaudiosink", "autoaudiosink");

    if (!audio_convert || !audio_resample || !volume_element || !autoaudiosink) {
        g_printerr("Failed to create audiobin elements\n");
        return;
    }

    gst_bin_add_many(GST_BIN(audiobin), audio_convert, audio_resample, volume_element, autoaudiosink, NULL);
    if (!gst_element_link_many(audio_convert, audio_resample, volume_element, autoaudiosink, NULL)) {
        g_printerr("Failed to link audiobin elements\n");
        return;
    }

    /* audiobin: expose sink ghost pad (so queue_audio -> audiobin works) */
    GstPad *audiobin_sink_raw = gst_element_get_static_pad(audio_convert, "sink");
    GstPad *audiobin_sink_ghost = gst_ghost_pad_new("sink", audiobin_sink_raw);
    gst_element_add_pad(audiobin, audiobin_sink_ghost);
    gst_object_unref(audiobin_sink_raw);

    /* ------------------ visualizer_bin ------------------ */
    visualizer_bin = gst_bin_new("visualizer_bin");
    GstElement *vis_convert = gst_element_factory_make("audioconvert", "vis_convert");
    GstElement *vis_resample = gst_element_factory_make("audioresample", "vis_resample");
    goom = gst_element_factory_make("goom", "goom");

    if (!vis_convert || !vis_resample || !goom) {
        g_printerr("Failed to create visualizer bin elements\n");
        return;
    }

    gst_bin_add_many(GST_BIN(visualizer_bin), vis_convert, vis_resample, goom, NULL);
    if (!gst_element_link_many(vis_convert, vis_resample, goom, NULL)) {
        g_printerr("Failed to link visualizer_bin elements\n");
        return;
    }

    /* visualizer_bin: expose sink ghost pad (to accept queue_visualizer -> visualizer_bin) */
    GstPad *vis_sink_raw = gst_element_get_static_pad(vis_convert, "sink");
    GstPad *vis_sink_ghost = gst_ghost_pad_new("sink", vis_sink_raw);
    gst_element_add_pad(visualizer_bin, vis_sink_ghost);
    gst_object_unref(vis_sink_raw);

    /* visualizer_bin: expose src ghost pad from goom so visualizer_bin -> videobin works */
    GstPad *goom_src_raw = gst_element_get_static_pad(goom, "src");
    if (!goom_src_raw) {
        g_printerr("Warning: goom has no 'src' pad (visualization might not produce video frames)\n");
    } else {
        GstPad *vis_src_ghost = gst_ghost_pad_new("src", goom_src_raw);
        gst_element_add_pad(visualizer_bin, vis_src_ghost);
        gst_object_unref(goom_src_raw);
    }

    /* ------------------ videobin ------------------ */
    videobin = gst_bin_new("videobin");
    queue_video_sink = gst_element_factory_make("queue", "queue_video_sink");
    videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
    gtksink = gst_element_factory_make("gtksink", "gtksink");

    if (!queue_video_sink || !videoconvert || !gtksink) {
        g_printerr("Failed to create videobin elements\n");
        return;
    }

    gst_bin_add_many(GST_BIN(videobin), queue_video_sink, videoconvert, gtksink, NULL);
    if (!gst_element_link_many(queue_video_sink, videoconvert, gtksink, NULL)) {
        g_printerr("Failed to link videobin elements\n");
        return;
    }

    /* videobin: expose sink ghost pad from queue_video_sink */
    GstPad *videobin_sink_raw = gst_element_get_static_pad(queue_video_sink, "sink");
    GstPad *videobin_sink_ghost = gst_ghost_pad_new("sink", videobin_sink_raw);
    gst_element_add_pad(videobin, videobin_sink_ghost);
    gst_object_unref(videobin_sink_raw);

    /* ------------------ Add elements to pipeline ------------------ */
    gst_bin_add_many(GST_BIN(pipeline),
                     filesrc, decodebin, tee,
                     queue_audio, audiobin,
                     queue_visualizer, visualizer_bin,
                     videobin,
                     NULL);

    /* Link filesrc -> decodebin */
    if (!gst_element_link(filesrc, decodebin)) {
        g_printerr("Failed to link filesrc -> decodebin\n");
        return;
    }

    /* connect decodebin pad-added; pass 'this' so handler can access instance members */
    g_signal_connect(decodebin, "pad-added", G_CALLBACK(PlayAudio::on_pad_added), this);

    /* Request tee src pads and link to queues */
    GstPad *tee_audio_pad = gst_element_get_request_pad(tee, "src_%u");
    GstPad *queue_audio_pad = gst_element_get_static_pad(queue_audio, "sink");
    if (gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link tee -> queue_audio\n");
    }
    gst_object_unref(tee_audio_pad);

    GstPad *tee_vis_pad = gst_element_get_request_pad(tee, "src_%u");
    GstPad *queue_vis_pad = gst_element_get_static_pad(queue_visualizer, "sink");
    if (gst_pad_link(tee_vis_pad, queue_vis_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link tee -> queue_visualizer\n");
    }
    gst_object_unref(tee_vis_pad);

    /* Link queue_audio -> audiobin (audiobin has sink ghost) */
    if (!gst_element_link(queue_audio, audiobin)) {
        g_printerr("Failed to link queue_audio -> audiobin\n");
    }

    /* Link queue_visualizer -> visualizer_bin */
    if (!gst_element_link(queue_visualizer, visualizer_bin)) {
        g_printerr("Failed to link queue_visualizer -> visualizer_bin\n");
    }

    /* Link visualizer_bin -> videobin (visualizer_bin has src ghost, videobin has sink ghost) */
    if (!gst_element_link(visualizer_bin, videobin)) {
        g_printerr("Failed to link visualizer_bin -> videobin\n");
    }

    g_print("Pipeline created successfully with audio + visualizer + video bins\n");
}

PlayAudio::~PlayAudio()
{
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
    }
}

/* pad-added: link decodebin new pad to tee's static sink pad */
void PlayAudio::on_pad_added(GstElement* decodebin_elem, GstPad* new_pad, gpointer user_data)
{
    PlayAudio *self = static_cast<PlayAudio*>(user_data);
    if (!self) return;

    GstCaps *caps = gst_pad_get_current_caps(new_pad);
    if (!caps) caps = gst_pad_query_caps(new_pad, NULL);
    if (!caps) {
        g_printerr("on_pad_added: could not get caps\n");
        return;
    }

    const GstStructure *str = gst_caps_get_structure(caps, 0);
    const gchar *name = gst_structure_get_name(str);

    /* For our design we always link decodebin -> tee (tee will split to audio/visualizer).
       Only caveat: if decodebin produces multiple pads, linking to the single tee sink is OK.
       However we still show debug which caps were linked. */
    GstPad *tee_sink = gst_element_get_static_pad(self->tee, "sink");
    if (!tee_sink) {
        g_printerr("on_pad_added: tee sink pad not found\n");
    } else {
        if (gst_pad_is_linked(tee_sink)) {
            g_print("tee sink already linked\n");
        } else {
            GstPadLinkReturn ret = gst_pad_link(new_pad, tee_sink);
            if (ret != GST_PAD_LINK_OK) {
                g_printerr("Failed to link decodebin new pad (%s) -> tee sink (ret=%d)\n", name, ret);
            } else {
                g_print("Linked decodebin pad (%s) -> tee sink\n", name);
            }
        }
        gst_object_unref(tee_sink);
    }

    gst_caps_unref(caps);
}




gboolean PlayAudio::on_message(GstBus *bus, GstMessage *msg, gpointer user_data) {
    PlayAudio *self = static_cast<PlayAudio*>(user_data);
  GtkWidget* window = self->getMainWindow(); // retrieve window


    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info = nullptr;  // Correctly declare debug_info

            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error: %s\n", err->message);

            if (debug_info) {
                g_print("Debug Info: %s\n", debug_info);
                g_free(debug_info);
            }

            g_error_free(err);
            break;
        }
        case GST_MESSAGE_EOS:
            g_print("******************End of stream. Playing next song******************\n");
             self->handleEOS(window);  // Play next song and update UI
            break;
        default:
            break;
    }

    return TRUE; // Keep watching for messages
}void PlayAudio::openFolderAndPlayFirstSong(const string &file_path)
{
    if (!pipeline) {
        g_printerr("Error: Pipeline is NULL\n");
        return;
    }
    
    if (!filesrc) {
        g_printerr("Error: filesrc is NULL\n");
        return;
    }

    if (file_path.empty()) {
        g_printerr("Error: file_path is empty\n");
        return;
    }

    g_print("Setting filesrc location to: %s\n", file_path.c_str());

    // Set the file path to the filesrc element's location property
    g_object_set(filesrc, "location", file_path.c_str(), NULL);


//watch bus for end-of-stream
      GstBus *bus = gst_element_get_bus(pipeline);           
    gst_bus_add_watch(bus, (GstBusFunc)on_message, this);    
    gst_object_unref(bus);    

    // Set pipeline state to playing
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Error: Pipeline failed to change state to PLAYING\n");
        return;
    }
  
}


void PlayAudio::play_audioFile(const string &file_path, const string &folder_path,GtkWidget* window)
{
    PlayAudio* instance = PlayAudio::getInstance();  // Use singleton instance

    cout << "Coming inside PlayAudio::playfile = " << folder_path << endl;

    // **STOP the currently playing stream before starting a new one**
    if (instance->pipeline) {
        gst_element_set_state(instance->pipeline, GST_STATE_NULL);
        g_print("Previous song completely stopped.\n");
    //     gst_element_set_state(instance->pipeline, GST_STATE_READY);
    //   g_print("Previous song reset to READY state.\n");

    }

    // Set the file path in the existing filesrc instead of recreating pipeline
    if (instance->filesrc) {
        g_object_set(instance->filesrc, "location", file_path.c_str(), NULL);
    } else {
        g_printerr("Error: filesrc is NULL\n");
        return;
    }

        // Reset seekbar and labels immediately
   GtkWidget *seekBox = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "seek_box"));
   GtkWidget *current_label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "current_time_label"));
   GtkWidget *total_label   = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "total_time_label"));
   if (seekBox) gtk_range_set_value(GTK_RANGE(seekBox), 0);
   if (current_label) gtk_label_set_text(GTK_LABEL(current_label), "00:00");
    if (total_label)   gtk_label_set_text(GTK_LABEL(total_label), "00:00");
std::string filename = file_path.substr(file_path.find_last_of("/\\") + 1);
    GtkWidget* songLabel = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
    
    if (songLabel) {
   // std::string filename = file_path.substr(file_path.find_last_of("/\\") + 1);
    gtk_label_set_text(GTK_LABEL(songLabel), filename.c_str());
    }
   

   // ✅ Update favourite/star icon
GtkWidget* favButton = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "fav_button"));
if (favButton) {
    auto it = std::find(favourite_songs.begin(), favourite_songs.end(), filename);

    const char* icon_name;
    if (it != favourite_songs.end()) {
        icon_name = "starred-symbolic";   // filled star if in favourites
        g_print("Song is in favourites → showing filled star\n");
    } else {
        icon_name = "non-starred-symbolic"; // empty star otherwise
        g_print("Song not in favourites → showing empty star\n");
    }

    GtkWidget *image = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_button_set_image(GTK_BUTTON(favButton), image);
}


    // Set pipeline state to playing
    GstStateChangeReturn ret = gst_element_set_state(instance->pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Error while changing pipeline state to PLAYING\n");
        return;
    }
        // Optional: wait until pipeline is fully in PLAYING (ensures duration available)
   gst_element_get_state(instance->pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
   
// ✅ Update pause icon using window
    GtkWidget* pauseButton = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "pause_button"));
    if (pauseButton) {
        GtkWidget *image = gtk_image_new_from_icon_name(
            "media-playback-pause", GTK_ICON_SIZE_SMALL_TOOLBAR
        );
        gtk_button_set_image(GTK_BUTTON(pauseButton), image);
    }



  

    GstBus *bus = gst_element_get_bus(instance->pipeline);
    gst_bus_add_watch(bus, (GstBusFunc)on_message, instance);

    gst_object_unref(bus);
}


bool PlayAudio::isPlaying() {
    if (!pipeline) return false;

    GstState state;
    gst_element_get_state(pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
    return (state == GST_STATE_PLAYING);
}

void PlayAudio::stopPipeline() {
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        GstState state;
        gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE); // wait until stopped
    }
}


// void PlayAudio::play_next()
// {
//     if (currently_playing_song_index + 1 < song_list.size()) {
//         currently_playing_song_index++;
       
//     } else {
//         g_print("******************End of playlist=playing fromg begining******************\n");
//         currently_playing_song_index=0;
        
        
//     }
//      string filepath=folder_Path+"/" +song_list[currently_playing_song_index];
//      //   cout<<filepath<<endl;
//     //    play_audioFile(filepath,folder_Path,window);
// }

void PlayAudio::handleEOS(GtkWidget* window) {
    std::string next_file;

    if (repeat_mode) {
        // Repeat current song regardless of shuffle
        next_file = folder_Path + "/" + song_list[currently_playing_song_index];
        g_print("Repeat mode ON → Replaying current song: %s\n", next_file.c_str());

    } else if (shuffle_mode) {
        // Pick a random index different from current
        if (song_list.size() > 1) {
            int rand_index;
            do {
                rand_index = rand() % song_list.size();
            } while (rand_index == currently_playing_song_index);

            currently_playing_song_index = rand_index;
        }
        // else only one song, keep the same index
        g_print("Shuffle mode ON → Playing random song index: %d\n", currently_playing_song_index);
        next_file = folder_Path + "/" + song_list[currently_playing_song_index];

    } else {
        // Sequential next song
        currently_playing_song_index++;
        if (currently_playing_song_index >= song_list.size()) {
            currently_playing_song_index = 0; // loop back to beginning
        }
        next_file = folder_Path + "/" + song_list[currently_playing_song_index];
    }

    // Play the next song
    play_audioFile(next_file, folder_Path, window);
}
