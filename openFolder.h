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
 void play_selected_item(const char *file_path,char* folder_path);
 void on_openFolder_clicked(GtkWidget *widget, gpointer data);
 

extern vector<string> song_list;

#endif // OPENFOLDER_H

