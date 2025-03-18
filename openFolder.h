#ifndef OPENFOLDER_H
#define OPENFOLDER_H

#include<iostream>
#include <gtk/gtk.h>
#include<string.h>
#include <vector>
#include <gst/gst.h>

#include<algorithm> //for sort
using namespace std;

// Declare the function
  static void play_selected_item(string &file_path,string  &folder_path);
 void on_openFolder_clicked(GtkWidget *widget, gpointer data);
 
extern int currently_playing_song_index; // Global index for the currently playing song //-1 =no song playing

extern vector<string> song_list;
extern string folder_Path; //so other files can use it

#endif // OPENFOLDER_H

