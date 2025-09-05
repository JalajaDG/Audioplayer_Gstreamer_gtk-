#ifndef PRINTPLAYLIST_H
#define PRINTPLAYLIST_H

#include<iostream>
#include <gtk/gtk.h>
#include<string.h>
#include <vector>
#include <gst/gst.h>


#include "openFolder.h"  //to use song_list vector ie is declared as extern in openFolder.h and that vector is filled in openFolder.cpp


using namespace std;
struct PlaylistData {
    GtkWidget* window;
    GtkWidget* dialog;
};


void ShowAudioFilesList(GtkWidget *widget, gpointer data);


#endif //PRINTPLAYLIST_H
