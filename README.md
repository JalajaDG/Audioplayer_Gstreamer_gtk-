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







	
