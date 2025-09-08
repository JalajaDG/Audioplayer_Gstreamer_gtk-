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
	    static PlayAudio* instance;  // Singleton instance

    GstElement *pipeline; // GStreamer pipeline	
	GstElement *filesrc;           // File source element
    GstElement *decodebin;        // Decoder element
    GstElement *audioconvert;     // Audio convert element
    GstElement *audioresample;    // Audio resample element
    GstElement *autoaudiosink;             // Audio output element
	GstBus *bus;
	GstMessage* msg;
	GtkWidget* main_window = nullptr;  // Store reference to main GTK window

public:
	PlayAudio();
	~PlayAudio();
	static PlayAudio* getInstance();  // Access Singleton instance
	bool isPlaying();

     void openFolderAndPlayFirstSong(const std::string& folder_path);
    GstElement* getPipeline() { return pipeline; } // Add this line to expose the pipeline


	void play_audioFile(const string &file_path,const string& folder_path,GtkWidget* window);
	static void on_pad_added(GstElement* element,GstPad* pad,gpointer user_data);
	void play_next();//folderpath is global no need to pass
	    static gboolean on_message(GstBus*, GstMessage*, gpointer);
		void handleEOS(GtkWidget* window);

	  // ✅ Add these setter and getter for main_window
    void setMainWindow(GtkWidget* window) { main_window = window; }
    GtkWidget* getMainWindow() const { return main_window; }

};
#endif  //PLAYAUDIO_H
