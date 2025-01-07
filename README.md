# GStreamer & GTK+ Audio Player


//check gstreamer version
gst-launch-1.0 --version
	gst-launch-1.0 version 1.16.3
	GStreamer 1.16.3

//check GTK+
pkg-config --modversion gtk+-3.0
	3.24.20


//GStreamer Development Libraries
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

//GTK+ Development Libraries
sudo apt install libgtk-3-dev


//pkg-config is a tool that helps in managing library flags during the compilation process. This is necessary for linking the right libraries for both GStreamer and GTK+
pkg-config --version
/* if not installed */sudo apt install pkg-config


//check for compiler and builder tools
sudo apt install build-essential

//You might also need Meson and Ninja for building some GTK+ applications (especially for the latest versions)
sudo apt install meson ninja-build

//check for audio suport
pulseaudio --version



//now all pre-requisites done,,check with a sample "test_gst.c file"

 $gcc -o test test_gst.c -I/usr/include/gtk-3.0 -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0 -L/usr/lib/x86_64-linux-gnu -lgtk-3 -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lpango-1.0 -lharfbuzz -lcairo
 
 $./test

