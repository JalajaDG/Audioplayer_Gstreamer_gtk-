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
 
 
 
 
 /* open the file explorer when button clicked*/
 ➡️static void on_button_clicked(GtkWidget *widget, gpointer data)
	◾️GtkWidget *widget: Represents the button that was clicked. This is provided automatically by GTK.
	◾️gpointer data: A generic pointer used to pass extra data to the function. In this case, it holds the reference to the main window 		  so the dialog can be tied to it.
	◾️GtkWidget *dialog: A variable to store the file chooser dialog widget that will be created.
	◾️GtkWidget *window = GTK_WIDGET(data): Converts the data pointer into a GtkWidget object representing the main window. This allows 		  us to associate  the dialog with the window, which improves the user experience (e.g., the dialog is positioned properly and    		  behaves like part of the app).
	◾️creating the file chooser dialog
		◾️gtk_file_chooser_dialog_new: Creates a file chooser dialog, a pop-up window where the user can select a folder.
		◾️dialog = gtk_file_chooser_dialog_new("Open Folder",
                                     GTK_WINDOW(window),
                                     GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                     "_Open", GTK_RESPONSE_ACCEPT,
                                     NULL);

		◾️"Open Folder": The title of the dialog box.
		◾️GTK_WINDOW(window): Links the dialog to the main window. This ensures the dialog is treated as a child of the main window.
		◾️GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER: Specifies that the dialog is for selecting a folder, not a file.
		◾️"_Cancel", GTK_RESPONSE_CANCEL: Adds a "Cancel" button to the dialog that returns a response code of GTK_RESPONSE_CANCEL 			  when clicked
		◾️"_Open", GTK_RESPONSE_ACCEPT: Adds an "Open" button that returns a response code of GTK_RESPONSE_ACCEPT when clicked.
		◾️NULL: Indicates the end of the button list.
	◾️add filter to  Filter for Audio
		◾️A GtkFileFilter is added to restrict the dialog to audio files. It filters files with extensions .mp3, .wav, and .ogg 			 using their MIME types.
		◾️gtk_file_filter_add_mime_type(filter, "audio/mpeg") allows .mp3 files.
		◾️gtk_file_filter_add_mime_type(filter, "audio/wav") allows .wav files.
		◾️gtk_file_filter_add_mime_type(filter, "audio/ogg") allows .ogg files.
		◾️ gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	◾️running the dialog
		◾️if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		◾️gtk_dialog_run: Displays the dialog and waits for the user to respond (e.g., by clicking "Open" or "Cancel").
		◾️GTK_RESPONSE_ACCEPT: If the user clicks the "Open" button, the function proceeds to handle their selection.
	◾️Getting the Selected Folder
		◾️char *folder_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		◾️gtk_file_chooser_get_filename: Retrieves the path of the folder selected by the user as a string.
		◾️folder_path: Stores the path to the selected folder.
	◾️. Printing the Selected Folder
		◾️g_print("Selected folder: %s\n", folder_path);
		◾️g_print: Outputs the selected folder path to the terminal or console for debugging or confirmation.
	◾️. Updating the Label
		◾️GtkWidget *label = g_object_get_data(G_OBJECT(window), "folder_label");
		◾️gtk_label_set_text(GTK_LABEL(label), folder_path);
		◾️g_object_get_data: Retrieves the widget (a label in this case) that was previously stored in the window object using the 				key "folder_label".
		◾️gtk_label_set_text: Updates the text displayed in the label to show the selected folder path.
	◾️. Iterate Over Files
		◾️if (dir): Ensures the directory was successfully opened before proceeding.
		◾️g_dir_read_name(dir): Reads the name of the next entry in the directory.
		◾️Returns the name of the file or directory (excluding the path) as a string.
		◾️Returns NULL when there are no more entries.
		◾️Filter Audio Files
			◾️g_str_has_suffix(file_name, ".mp3") ||
		◾️construct full file path
			◾️string file_path = string(folder_path) + "/" + string(file_name);
	◾️add to playlist
		◾️playlist.push_back(file_path);
	◾️sort the vector =to see the music list as in normal folder
	◾️close directory =g_dir_close(dir);

	◾️Freeing the Folder Path Memory
		◾️g_free(file_path);
		◾️g_free: Frees the memory allocated for the folder_path. GTK allocates memory dynamically for paths, and it's the 			  programmer's responsibility to release it to avoid memory leaks.
	
	◾️Destroying the Dialog
		◾️gtk_widget_destroy(dialog);

	
➡️ g_object_set_data(G_OBJECT(window), "folder_label", label);
 	◾️if a button click should update a label, progress bar, or any other widget in the window, you can store a reference to that 	 widget using g_object_set_data and retrieve it in the callback.

	◾️The reason you use g_object_set_data to store a reference to a widget (like a label or progress bar) is that GTK callbacks often 		 do not automatically have access to widgets other than the one that triggered the event (e.g., the button).
	
	◾️ Why Use g_object_set_data?
		◾️g_object_set_data allows you to "attach" data (like the reference to the label) to another widget (like the window). 			 This works   well because:
			◾️The window object is accessible from most parts of the program.
			◾️You don't need to pass the label explicitly to every callback function.
			◾️It's clean and avoids creating a global variable to hold the label.
	◾️How g_object_set_data Works
		◾️It associates a "key-value" pair with a widget.
		◾️The key is a string (e.g., "folder_label"), and the value is a pointer to the widget.
		◾️Later, you can retrieve the associated widget using g_object_get_data
	

➡️TO SET THE HEIGHT AND WIDTH OF FOLDERBOX,
	◾️INSTEAD Of these;
		◾️ gtk_widget_set_size_request(button, 10, 10);
		◾️gtk_widget_set_size_request(folderBox, 10, 10); // WIDTH,HEIGHT
	◾️use;
		 ◾️gtk_box_pack_start(GTK_BOX(vbox), folderBox, FALSE, FALSE, 0); // Add label to the top partition
		 ◾️set to false,false.
		 ◾️ void gtk_box_pack_start(GtkBox *box,
                        GtkWidget *child,
                        gboolean expand,
                        gboolean fill,
                        guint padding);
                   ◾️expand (gboolean):This determines whether the widget should expand to take up any remaining space in the container.
		 ◾️fill (gboolean):This determines whether the widget should "fill" the available space within its allocated size. If 			   expand is TRUE, this decides if the widget fills the extra space.
		 ◾️padding=The amount of space (in pixels) to add between this widget and the other widgets in the GtkBox. 
		 
		 
		 
		 
		 
		 
		 
		 
➡️icons
 sudo apt-get install gtk-3-examples
gtk3-icon-browser
GtkWidget *ShowAudioFilesIcon = gtk_button_new_from_icon_name("multimedia-volume-control", GTK_ICON_SIZE_SMALL_TOOLBAR);



➡️ShowAudioFilesList
	◾️GtkWidget *dialog;: Declares a variable for the dialog widget.
	◾️GtkWidget *content_area;: Declares a variable for the dialog's content area widget.
	◾️GtkWidget *scrolled_window;: Declares a variable for the scrolled window widget (a container that adds scrollbars to its child 	    widgets).
	◾️GtkWidget *list_box;: Declares a variable for the list box widget that will display the audio files.
	◾️GtkWidget *window = GTK_WIDGET(data);: Converts the data parameter (passed to the function) to a GtkWidget pointer, assuming it's 		 the main application window.
	◾️dialog = gtk_dialog_new_with_buttons("Audio Files",
                                     GTK_WINDOW(window),
                                     GTK_DIALOG_MODAL,
                                     "_Close", GTK_RESPONSE_CLOSE,
                                     NULL);
                  ◾️gtk_dialog_new_with_buttons: Creates a new dialog window.
		◾️"Audio Files": The title of the dialog.
		◾️GTK_WINDOW(window): The parent window for the dialog.
		◾️GTK_DIALOG_MODAL: Makes the dialog modal, meaning it blocks interaction with the parent window while open.
		◾️"_Close", GTK_RESPONSE_CLOSE: Adds a "Close" button to the dialog with a GTK_RESPONSE_CLOSE response ID.
		◾️NULL: No additional buttons are added.
	◾️Get the Content Area:
		◾️content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
		◾️gtk_dialog_get_content_area: Retrieves the content area of the dialog, where additional widgets can be placed.

	◾️Add a Scrolled Window
		◾️gtk_scrolled_window_new(NULL, NULL): Creates a new scrolled window with default adjustments (no custom vertical or 		horizontal adjustments).
		◾️gtk_scrolled_window_set_policy: Sets the scroll policy for the window.
			◾️GTK_POLICY_AUTOMATIC: Scrollbars are added automatically when content exceeds the visible area.
		◾️gtk_widget_set_size_request: Sets a size of 400x300 pixels for the scrolled window.
		◾️gtk_container_add: Adds the scrolled window to the dialog's content area.
	◾️Create a List Box
		◾️gtk_list_box_new(): Creates a new GtkListBox widget, which is used to display a vertical list of items.
		◾️gtk_container_add: Adds the list box to the scrolled window.

	◾️Add Playlist Items

		◾️for (const string &song : playlist): Iterates over the playlist vector, where each song is a string representing the f		ile path of an audio file.
		◾️gtk_label_new(song.c_str()): Creates a label widget to display the song text.
		◾️song.c_str(): Converts the C++ string to a C-style string (required by GTK functions).
		◾️gtk_list_box_insert: Adds the label to the list box.
		◾️-1: Inserts the label at the end of the list
	◾️gtk_widget_show_all(dialog);
	◾️gtk_dialog_run(GTK_DIALOG(dialog));=Runs the dialog in a blocking mode, waiting for user interaction
	◾️gtk_widget_destroy(dialog);
	◾️while sperating this method() into openFOlder.cpp =Static functions are only visible within the file they are defined in, so their declaration should not be in the header 			file.
		◾️both openFolder.h and openFolder.cpp cant have as static void on_opeFolder_clicked()= bcz.
		◾️A static function in C++ has internal linkage. This means the function is visible only within the file where it is defined.
		◾️The linker does not see static functions across different translation units (.cpp files). Therefore, you cannot call a static function from one .cpp file in another.




//using playbin
➡️start to play the song when new folder opened and clicked a song..inside on_button_clicked->on dialog run =play_selected_song
	◾️include gstreamer library=#include <gst/gst.h>
	◾️initialize gstreamer=     gst_init(&argc, &argv);
	◾️include gstreamer-1.0 flag =//g++ `pkg-config --cflags gtk+-3.0 gstreamer-1.0` audioPlayer.cpp -o ap `pkg-config --libs gtk+-3.0 gstreamer-1.0`
	◾️error=The g_object_get_data function returns a gpointer (a void*), which needs to be explicitly cast to the correct type
		GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));
	◾️ play_selected_song(full_path.c_str());=You don't necessarily need to use a C-style string (const char*). You can pass a std::string to the play_selected_song method as well. 			However, since GStreamer expects a const char* for the URI parameter,

	 ◾️   if (playbin == NULL)=  meaning it hasn't been created yet.
	◾️Create the playbin element:    
		◾️playbin = gst_element_factory_make("playbin", "playbin");
			It creates a new element based on the specified factory type (in this case, "playbin") and a name ("playbin"). The "playbin" element is a simple player that 				can handle multimedia content (audio or video).
	◾️Check if playbin creation failed
	◾️Set the URI for the file to be played:
		    ◾️g_object_set(playbin, "uri", g_strdup_printf("file://%s", file_path), NULL);
			◾️g_object_set: This is a GObject function that sets the properties of a GObject (in this case, playbin). Here, it sets the uri property of the playbin 				element.
			◾️playbin: This is the GObject (GStreamer element) whose property is being set.
			◾️"uri": This is the name of the property that specifies the URI (Uniform Resource Identifier) of the file to be played. GStreamer uses this to know the 			location of the media file.
			◾️g_strdup_printf("file://%s", file_path): This function creates a new string from the format file:// followed by the provided file_path. The g_strdup_printf 				function works like sprintf, but it allocates memory for the resulting string and returns a pointer to it. The file:// prefix is necessary to specify the URI 				scheme for file-based media.
			◾️g_strdup_printf is a function from the GLib library that behaves similarly to the C printf
			◾️g_strdup_printf("file://%s", file_path) takes the file_path (which might be something like /home/user/music/song.mp3) and combines it with the file:// 				prefix to create a URI string.
			◾️NULL: The last parameter of g_object_set is terminated by NULL, as it accepts a list of key-value pairs.
	◾️Set the playbin state to playing:
	    	◾️GstStateChangeReturn ret = gst_element_set_state(playbin, GST_STATE_PLAYING);
			◾️gst_element_set_state: This function changes the state of a GStreamer element. Elements have different states (like GST_STATE_NULL, GST_STATE_READY, 				◾️GST_STATE_PAUSED, and GST_STATE_PLAYING), which define the element's behavior.
			◾️playbin: This is the GStreamer element whose state is being changed.
			◾️GST_STATE_PLAYING: This is the target state. It means the element should start playing the media.
			◾️GstStateChangeReturn: This is the return type of gst_element_set_state. It indicates the result of the state change, such as GST_STATE_CHANGE_FAILURE if the 					state change fails.
	◾️Check if the state change was successful:    if (ret == GST_STATE_CHANGE_FAILURE) {
	




➡️ShowAudioFilesList
	◾️seperate method in files printPlaylist.h and printPlaylist.cpp
	◾️here as we know this uses song_list vector ..the contents to this vector  is filled inside  openfolder.cpp so include openfolder.h (in that song_list is defined as extern )
	◾️Make the list items interactive using gtk_list_box_set_activate_on_single_click().
	◾️ gtk_list_box_set_activate_on_single_click(list_box, TRUE); // Enable click activation
    
   ◾️ g_signal_connect(list_box, "row-activated", G_CALLBACK(on_row_activated), data)//// Connect the "row-activated" signal to the callback function
	◾️ on_row_activated=to handle row activation (when a song is clicked)


➡️NNow in openFOlder.cpp 
	>we must play song without using playbin
	>so first check if its a mp3 file or not?
	>


➡️playing local audio file
	>to play local audio files, a typical pipeline includes these elements:filesrc,decodebin,audioconvert,audioresample,autoaudiosink
		◾️filesrc: Reads the audio file from the filesystem.
		◾️decodebin: Automatically detects and decodes the file format.
		◾️audioconvert: Converts the audio to a format that can be processed further.
		◾️audioresample: Resamples the audio if needed to match the requirements of the sink.
		◾️autoaudiosink: Automatically chooses the appropriate audio output (e.g., speakers or headphones).
	➡️steps
		◾️1.initialize gstreamer
		◾️2.cretae pipeline
		◾️3. Create Elements (filesrc, decodebin, audioconvert,audioresample, autoaudiosink).
		◾️4.Add Elements to the Pipeline
		◾️5.Link the elements (static and dynamic linking).  (refer below abt static and dynamic linking)
		◾️ 6.Set the File Source
		◾️7.Start Playback=Set the pipeline state to PLAYING.
		◾️8.Add a main loop to keep the application running.
		◾️ 9.Cleanup
>discuss y audioconvert and audioresample is important?
		◾️Suppose you have a high-resolution audio file with the following properties:
		  Audio File Details
			Format: PCM 24-bit (signed 24-bit little-endian)
			Sample Rate: 96,000 Hz (96 kHz)
			Channels: 2 (Stereo)
		 Audio Sink Capabilities:Your audio sink (autoaudiosink, alsasink, or pulsesink) only supports:
			Format: PCM 16-bit (signed 16-bit little-endian)
			Sample Rate: 44,100 Hz (44.1 kHz)
			Channels: 2 (Stereo)
		◾️This is common with many consumer-grade sound cards or built-in speakers, which don't support high-res audio.
		◾️If you construct the pipeline without conversion:
		 gst-launch-1.0 filesrc location=high_res_audio.wav ! decodebin ! autoaudiosink
			◾️ERROR: from element /GstPipeline:pipeline0/GstAutoAudioSink:audiosink0: Could not negotiate format
			  y error=
				◾️filesrc reads the file.
				◾️decodebin decodes the audio into raw PCM data with its original properties (24-bit, 96 kHz).
				◾️autoaudiosink tries to negotiate a format it supports (16-bit, 44.1 kHz), but since the input is in 24-bit/96 kHz and no intermediate element is 					 handling the conversion, the pipeline fails with an error like:

		◾️Pipeline With audioconvert and audioresample
			gst-launch-1.0 filesrc location=high_res_audio.wav ! decodebin ! audioconvert ! audioresample ! autoaudiosink
				◾️filesrc reads the high-res audio file.
				◾️decodebin decodes it into raw PCM audio with properties:   audio/x-raw, format=S24LE, rate=96000, channels=2
				◾️audioconvert checks the capabilities of the downstream element (autoaudiosink).
					It detects that autoaudiosink supports 16-bit audio but not 24-bit.
					It converts the audio from S24LE (24-bit PCM) to S16LE (16-bit PCM).

				◾️The audio caps after conversion become:
					audio/x-raw, format=S16LE, rate=96000, channels=2

				◾️audioresample checks that the sink requires a 44.1 kHz sample rate.It resamples the audio from 96,000 Hz to 44,100 Hz.
					audio/x-raw, format=S16LE, rate=44100, channels=2
				◾️The sink receives audio that matches its capabilities (16-bit, 44.1 kHz, stereo) and plays it without issues.

>how audioconvert suppports multichannel audio i/p to monochannel audio as o/p?
        Watching a 5.1 Surround Movie on Headphones:
	◾️Input: 6 channels (5.1 surround)
	  Output: 2 channels (stereo)
	◾️gst-launch-1.0 filesrc location=surround_sound.wav ! decodebin ! audioconvert ! "audio/x-raw,channels=2" ! autoaudiosink
		◾️decodebin extracts the raw 5.1 audio.
		◾️audioconvert detects that the downstream (autoaudiosink) only supports stereo.
		◾️audioconvert performs downmixing:
			◾️Combines the six channels into two (left and right) while preserving balance and spatial information.
			◾️Ensures bass and dialog are included in the stereo output.
		◾️Why Downmixing Is Necessary
			◾️Compatibility:Stereo speakers cannot play multi-channel audio directly. Downmixing ensures the audio can still be heard.
			◾️Without downmixing, critical elements like dialog (center channel) or bass (LFE) might be lost entirely.
			◾️Balanced Listening:Downmixing ensures a pleasant listening experience by distributing all channels proportionally.
		◾️Debugging Downmixing in GStreamer
			◾️GST_DEBUG=audioconvert:5 gst-launch-1.0 filesrc location=surround_sound.wav ! decodebin ! audioconvert ! "audio/x-raw,channels=2" ! autoaudiosink
				To debug how channels are being mixed, you can inspect the audio caps and see how the audioconvert element processes them.
			◾️GST_DEBUG=audioconvert:5
				Purpose: Enables detailed logging for the audioconvert element.
				Debug Level (5): The highest debug level, showing detailed information about how audioconvert processes the audio.
				Output: Logs are printed to the terminal, showing input/output caps (capabilities) and processing steps.
			◾️"audio/x-raw,channels=2"
				Caps Filter: "audio/x-raw,channels=2"
				Specifies that the audio output must have 2 channels (stereo).
				audio/x-raw: Indicates raw, uncompressed audio data.
				channels=2: Restricts the output to 2 audio channels.
			◾️Sample Debug Output:
				audioconvert0: Input caps = audio/x-raw, format=S16LE, rate=48000, channels=6
				audioconvert0: Downmixing 5.1 channels to 2 channels
				audioconvert0: Output caps = audio/x-raw, format=S16LE, rate=48000, channels=2
			



➡️Linking elements inside bin(after adding all elements inside bin)
| **Link**        | **From Element**     | **To Element**       | **Link Type**  | **Reason**                                                                                     |
|-----------------|----------------------|----------------------|----------------|-------------------------------------------------------------------------------------------------|
| Static Link     | filesrc              | decodebin            | Static         | The source (`filesrc`) directly passes the file to `decodebin` for decoding.                 |
| Dynamic Link    | decodebin            | audioconvert         | Dynamic        | `decodebin` has a dynamic pad that changes based on the decoded format. We use the `pad-added` signal
									|to connect it to `audioconvert`.
 									| 
| Static Link     | audioconvert         | audioresample        | Static         | `audioconvert` outputs a compatible format which is resampled by `audioresample`.              |
| Static Link     | audioresample        | autoaudiosink        | Static         | After the audio is resampled, it is sent to `autoaudiosink` for playback.                     |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
◾️Static Link: These are elements that can be linked directly because their input/output pads are fixed. For example, filesrc (the source) can be directly linked to decodebin (the decoder), and audioconvert can be linked to audioresample, which then links to autoaudiosink.
	◾️gst__element_link(,);
	◾️gst__element_link_many(, , ,NULL);	
	
◾️Dynamic Link: This occurs because some elements, like decodebin, have dynamic pads that are created during runtime. The pad is not known at compile time, so we need to listen for the pad-added signal to link it to the next element (audioconvert).
	◾️g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), audioconvert);
	◾️parameters;
		-obj emitting signal
		-signal name
		-pointer to callback when signal is emmiteed
		-gpointer data-any data to be passed to callback function()
	
	◾️This connects the "pad-added" signal of decodebin to the callback function.The callback function is invoked whenever a new pad is created by decodebin.
	◾️The callback function is responsible for linking the dynamically created pad from decodebin to the next element, e.g., audioconvert.
	◾️In the callback, get the static sink pad of the next element (audioconvert).
	◾️Link the dynamically created pad to the static pad of the next element.
	◾️static void on_pad_added(GstElement *element, GstPad *pad, gpointer user_data)
	-parameters;
		-GStreamer element that emitted the "pad-added" signal.
		-This is the newly added pad from the decodebin element.
		-additional data passed


➡️ playAudio.cpp
 void play_audioFile(const string &file_path)
 	◾️here & bcz pass by reference,instead of pass by value, to  avoid copying the entire string
 	◾️By using &, you're passing the original string directly to the function without the overhead of copying
 	◾️const bcz=When you use const (like const std::string& file_path), you promise not to modify the string inside the function. This guarantees that the function won’t 		accidentally change the value of the file_path passed to it.
 	◾️now set filesrc elements location property to this passed filepath using  = g_object_set()
 	◾️set pipeline to playing set,and check
	◾️add bus to check eos=   Bus=gst_element_get_bus(pipeline);
	◾️get msg  from bus
	◾️    // Handle errors or end of stream (EOS)
	◾️unref msg,bus;
	◾️In your play_audioFile() function, this line is causing the issue:
		msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, 
     	 static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
	◾️ this code used a blocking call to wait for messages on the GStreamer bus:
	◾️It caused the program to block indefinitely until an error (GST_MESSAGE_ERROR) or end-of-stream (GST_MESSAGE_EOS) occurred,This froze the UI and prevented other interactions (such as updating the playlist or UI).

	◾️This blocks execution until the song finishes playing (or an error occurs), preventing your UI (printplaylist )from responding.
	◾️Replace=
	the code uses a non-blocking approach with gst_bus_add_watch:
	◾️Instead of blocking, it registers a callback function (on_message) that gets triggered when a message (error or EOS) is received.
	◾️The program continues execution without freezing.
	◾️When a song ends, on_message() automatically calls play_next() to play the next song.




➡️ openFolder.cpp
use a thread for calling  =play_selected_item()  or else..it will block next line(print song vector,and file explorer window wont close)

	

➡️ for handling EOS inside playAudio.cpp
◾️currently_playing_song_index=make it extern global
◾️to use a same variable  one multiple files...we cant just make it global bcz.it will be only within that file...thats y make it extern
◾️rules to make a variable extern and use it across multiple files
	◾️Use the extern keyword in header files (e.g., x.h) to declare the variable. This tells the compiler that the variable exists, but it is defined elsewhere
	◾️inside the .cpp file u need to declare it as ....int variable;








