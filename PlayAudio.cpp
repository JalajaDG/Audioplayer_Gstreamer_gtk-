#include "PlayAudio.h"


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

//check if all elements created

    if (!filesrc || !decodebin || !audioconvert || !audioresample || !autoaudiosink) {
        g_printerr("Failed to create one or more audio pipeline elements.\n");
        return;
    }

//add elements inside pipeline
gst_bin_add_many(GST_BIN(pipeline),filesrc,decodebin,audioconvert,audioresample,autoaudiosink,NULL);


    // Link the elements 

    // (static linking for source to decodebin,
    // dynamic link fo decodebin to audioconvert..using pad added signal,
    // dynamic link fo audioconvert to audioresample,
    // dynamic link fo audioresample to autoaudiosink)


   if(!gst_element_link(filesrc,decodebin))
   {
    g_printerr("failed to link audio elements = sorce and decodebin\n");
    return;
   }

  // Connect "pad-added" signal to the decodebin element
  g_signal_connect(decodebin,"pad-added",G_CALLBACK(on_pad_added),audioconvert);

   if(!gst_element_link_many(audioconvert,audioresample,autoaudiosink,NULL))
   {
     g_printerr("failed to link audio elements = audioconvert,audioresample  and sink\n");
    return;
   }





}

//destructor
PlayAudio::~PlayAudio()
{
 // Clean up and stop the pipeline if running
if(pipeline)
{
  gst_element_set_state(pipeline,GST_STATE_NULL);
gst_object_unref(pipeline);

}




}

void PlayAudio::  on_pad_added(GstElement* element,GstPad* pad,gpointer user_data)
{
    GstElement *audioconvert = (GstElement *)user_data;

     // Get the sink pad of audioconvert
    GstPad *sinkpad=gst_element_get_static_pad(audioconvert,"sink");

    // Link the dynamic pad from decodebin to audioconvert
    if(gst_pad_link(pad,sinkpad)!= GST_PAD_LINK_OK)
    {
     g_printerr("Failed to link pads of decodebin and audioconvert in audio pipeline!\n");
    }
  // Release the sinkpad reference
  gst_object_unref(sinkpad);

}
  void PlayAudio::play_audioFile(const string &file_path)
  {
    // Set the file path to the filesrc element's location property
   g_object_set(filesrc,"location",file_path.c_str(),NULL);
    // Set the file path to the filesrc element's location property
    GstStateChangeReturn ret= gst_element_set_state(pipeline,GST_STATE_PLAYING);
  if(ret==GST_STATE_CHANGE_FAILURE)
   {
   g_printerr(" audio pipeline= pipeline errro while changing its state to playing");
   return;
    }
    // Wait for the pipeline to finish playing (blocking the thread here)

   bus=gst_element_get_bus(pipeline);
   msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, static_cast<GstMessageType>( GST_MESSAGE_ERROR | GST_MESSAGE_EOS));



    // Handle errors or end of stream (EOS)
    if (msg != NULL) {
        GError* err;
        gchar* debug_info;

         switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error: %s\n", err->message);
                g_error_free(err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End of stream\n");
                break;
            default:
                // Unexpected message type
                break;
        }
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
  }