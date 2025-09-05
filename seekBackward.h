#ifndef SEEKBACKWARD_H
#define SEEKBACKWARD_H
#include<iostream>
#include <gtk/gtk.h>
#include<string.h>
#include <vector>
#include <gst/gst.h>


#include "openFolder.h"  //to use song_list vector ie is declared as extern in openFolder.h and that vector is filled in openFolder.cpp


void on_skip_backward_clicked(GtkWidget *widget, gpointer data); //widget is skipbackward button,data is window that is passed from g_signal_connect


#endif //SEEKBACKWARD_H


