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
 {
   //initialize gstreamer
   gst_init(NULL,NULL);
 
   // Create the pipeline
    pipeline=gst_pipeline_new("audio-pipeline");
 
    if(!pipeline)
    {
     g_printerr("Failed to create audio pipeline ");
     return;
    }
  
 
        // Create the GStreamer elements
   filesrc=gst_element_factory_make("filesrc","filesrc");
   decodebin=gst_element_factory_make("decodebin","decodebin");
   audioconvert=gst_element_factory_make("audioconvert","audioconvert");
   audioresample=gst_element_factory_make("audioresample","audioresample");
   autoaudiosink=gst_element_factory_make("autoaudiosink","autoaudiosink");
volume_element = gst_element_factory_make("volume", "volume");

//  Create the  elements for visualizer
tee        = gst_element_factory_make("tee", "audio_tee");
queue_audio = gst_element_factory_make("queue", "queue_audio");
queue_goom  = gst_element_factory_make("queue", "queue_goom");
goom        = gst_element_factory_make("goom", "goom");
gtksink     = gst_element_factory_make("gtksink", "gtksink");
 
 //check if all elements created
 
     if (!filesrc || !decodebin || !audioconvert || !audioresample ||!volume_element|| !autoaudiosink) {
         g_printerr("Failed to create one or more audio pipeline elements.\n");
         return;
     }
     // Check all audio-visualizer elements created
    if (!tee || !queue_audio || !queue_goom || !goom || !gtksink) {
    g_printerr("Failed to create visualizer elements\n");
    return;
}
 
 //add all elements (audio + visualizer) to the pipeline

 //gst_bin_add_many(GST_BIN(pipeline),filesrc,decodebin,audioconvert,audioresample,volume_element,autoaudiosink,NULL);

 gst_bin_add_many(GST_BIN(pipeline),
    filesrc, decodebin,
    audioconvert, audioresample,
    tee,                // tee after audioresample
    queue_audio, volume_element, autoaudiosink,  // audio branch
    queue_goom, goom, gtksink,                  // visualizer branch
    NULL);
     // Link the elements 
 
     // (static linking for source to decodebin,
     // dynamic link fo decodebin to audioconvert..usifng pad added signal,
     // dynamic link fo audioconvert to audioresample,
     // dynamic link fo audioresample to autoaudiosink)
 
 
    if(!gst_element_link(filesrc,decodebin))
    {
     g_printerr("failed to link audio elements = sorce and decodebin\n");
     return;
    }
 
   // Connect "pad-added" signal to the decodebin element
   g_signal_connect(decodebin,"pad-added",G_CALLBACK(on_pad_added),audioconvert);

 
    // Static link: audioconvert → audioresample → tee
if (!gst_element_link_many(audioconvert, audioresample, tee, NULL)) {
    g_printerr("Failed to link audioconvert → audioresample → tee\n");
    return;
}

 // Tee branches via request pads
  GstPad *tee_audio_pad = gst_element_get_request_pad(tee, "src_%u");
    GstPad *queue_audio_pad = gst_element_get_static_pad(queue_audio, "sink");
    if (gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link tee → queue_audio\n");
        return;
    }

    GstPad *tee_goom_pad = gst_element_get_request_pad(tee, "src_%u");
    GstPad *queue_goom_pad = gst_element_get_static_pad(queue_goom, "sink");
    if (gst_pad_link(tee_goom_pad, queue_goom_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link tee → queue_goom\n");
        return;
    }

    // Link remaining elements in branches
    if (!gst_element_link_many(queue_audio, volume_element, autoaudiosink, NULL)) {
        g_printerr("Failed to link audio branch queue → volume → sink\n");
        return;
    }

    if (!gst_element_link_many(queue_goom, goom, gtksink, NULL)) {
        g_printerr("Failed to link visualizer branch queue → goom → gtksink\n");
        return;
    }

    g_print("Pipeline created successfully with audio + Goom visualizer\n");


 

 
 }
//destructor
PlayAudio::~PlayAudio()
{
 // Clean up and stop the pipeline if running
if(pipeline)
{
  gst_element_set_state(pipeline,GST_STATE_NULL);
gst_object_unref(pipeline);
        pipeline = NULL; // Prevent using old reference


}




}




void PlayAudio::  on_pad_added(GstElement* element,GstPad* pad,gpointer user_data)
{
    GstElement *audioconvert = (GstElement *)user_data;

     // Get the sink pad of audioconvert
    GstPad *sinkpad=gst_element_get_static_pad(audioconvert,"sink");

    //  // Print pad capabilities for debugging
    // GstCaps *caps = gst_pad_query_caps(pad, NULL);
    // g_print("Pad capabilities: %s\n", gst_caps_to_string(caps));
    // gst_caps_unref(caps);

    // Link the dynamic pad from decodebin to audioconvert
    if(gst_pad_link(pad,sinkpad)!= GST_PAD_LINK_OK)
    {
     g_printerr("Failed to link pads of decodebin and audioconvert in audio pipeline!\n");
    }
  // Release the sinkpad reference
  gst_object_unref(sinkpad);
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
