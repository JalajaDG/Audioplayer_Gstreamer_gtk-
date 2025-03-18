#ifndef PLAYAUDIO_H
#define PLAYAUDIO_H
#include<bits/stdc++.h>
#include<iostream>
#include <gtk/gtk.h>
#include<string.h>
#include <vector>
#include <gst/gst.h>
#include <gst/gstelement.h>  // For gst_element_unref


#include <gst/gst.h>
using namespace std;

extern  int currently_playing_song_index; 

class PlayAudio
{
private:
    GstElement *pipeline; // GStreamer pipeline	
	GstElement *filesrc;           // File source element
    GstElement *decodebin;        // Decoder element
    GstElement *audioconvert;     // Audio convert element
    GstElement *audioresample;    // Audio resample element
    GstElement *autoaudiosink;             // Audio output element
	GstBus *bus;
	GstMessage* msg;
public:
	PlayAudio();
	~PlayAudio();
	
     void openFolderAndPlayFirstSong(const std::string& folder_path);


	void play_audioFile(const string &file_path,const string& folder_path);
	static void on_pad_added(GstElement* element,GstPad* pad,gpointer user_data);
	void play_next();//folderpath is global no need to pass
	    static gboolean on_message(GstBus*, GstMessage*, gpointer);



};
#endif  //PLAYAUDIO_H
