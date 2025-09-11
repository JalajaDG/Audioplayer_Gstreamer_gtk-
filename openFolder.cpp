#include "openFolder.h"
#include "PlayAudio.h"
#include <thread>
#include <algorithm>
#include <iostream>

using namespace std;

vector<string> song_list; // Global song list
string folder_Path;        // Global folder path

// Forward declarations
static void play_selected_item(const string &full_path, const string &folder_path, GtkWidget *window);
static void handle_folder_change_and_play(const string &full_path, const string &new_folder_path, GtkWidget *window);

// ==================================================
// Function to handle folder change and play song
// ==================================================
static void handle_folder_change_and_play(const string &full_path, const string &new_folder_path, GtkWidget *window)
{
    PlayAudio *player = PlayAudio::getInstance();

    if (folder_Path != new_folder_path)
    {
        g_print("Folder changed → stopping current pipeline: %s\n", new_folder_path.c_str());
        folder_Path = new_folder_path; // update global folder
    }

    // Play the selected file using existing pipeline
    player->play_audioFile(full_path, new_folder_path, window);
}


// ==================================================
// Play selected item
// ==================================================
static void play_selected_item(const string &full_path, const string &folder_path, GtkWidget *window)
{
    if (g_str_has_suffix(full_path.c_str(), ".mp3") ||
        g_str_has_suffix(full_path.c_str(), ".wav") ||
        g_str_has_suffix(full_path.c_str(), ".ogg"))
    {
        PlayAudio *player = PlayAudio::getInstance();
        g_print("Playing song: %s\n", full_path.c_str());
        player->openFolderAndPlayFirstSong(full_path);

        GtkWidget *song_label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
        if (song_label)
        {
            const string filename = song_list[currently_playing_song_index];
            cout << "Now playing (from vector): " << filename << endl;
            gtk_label_set_text(GTK_LABEL(song_label), filename.c_str());
        }
        else
        {
            cout << "⚠️ song_label not found in window!" << endl;
        }
    }
}

// ==================================================
// Open default folder
// ==================================================
void open_default_folder(GtkWidget *window)
{
    const string default_path = "/home/ee213082/Music";
    g_print("Opening default folder: %s\n", default_path.c_str());

    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));
    if (label)
        gtk_label_set_text(GTK_LABEL(label), default_path.c_str());

    GDir *dir = g_dir_open(default_path.c_str(), 0, NULL);
    if (!dir)
    {
        g_print("⚠️ Failed to open default folder.\n");
        return;
    }

    const gchar *file_name;
    song_list.clear();

    while ((file_name = g_dir_read_name(dir)) != NULL)
    {
        if (g_str_has_suffix(file_name, ".mp3") ||
            g_str_has_suffix(file_name, ".wav") ||
            g_str_has_suffix(file_name, ".ogg"))
        {
            song_list.push_back(file_name);
        }
    }

    g_dir_close(dir);
    sort(song_list.begin(), song_list.end());

    if (!song_list.empty())
    {
        currently_playing_song_index = 0;
        folder_Path = default_path;
        const string full_path = folder_Path + "/" + song_list[currently_playing_song_index];

        std::thread play_thread([full_path, folder_Path, window]() {
            handle_folder_change_and_play(full_path, folder_Path, window);
        });

        play_thread.detach();
    }
    else
    {
        g_print("⚠️ No songs found in default folder.\n");
    }
}

// ==================================================
// Callback for open folder button
// ==================================================
void on_openFolder_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *window = GTK_WIDGET(data);
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open Folder",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *folder_path_c = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        const string folder_path = folder_path_c;
        g_print("Selected folder: %s\n", folder_path.c_str());

        GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));
        if (label)
            gtk_label_set_text(GTK_LABEL(label), folder_path.c_str());

        GDir *dir = g_dir_open(folder_path.c_str(), 0, NULL);
        if (dir != NULL)
        {
            const gchar *file_name;
            song_list.clear();

            while ((file_name = g_dir_read_name(dir)) != NULL)
            {
                if (g_str_has_suffix(file_name, ".mp3") ||
                    g_str_has_suffix(file_name, ".wav") ||
                    g_str_has_suffix(file_name, ".ogg"))
                {
                    song_list.push_back(file_name);
                }
            }

            g_dir_close(dir);
            sort(song_list.begin(), song_list.end());
        }
        else
        {
            g_print("Failed to open the directory: %s\n", folder_path.c_str());
        }

        if (!song_list.empty())
        {
            currently_playing_song_index = 0;
            const string full_path = folder_path + "/" + song_list[currently_playing_song_index];

            std::thread play_thread([full_path, folder_path, window]() {
                handle_folder_change_and_play(full_path, folder_path, window);
            });

            play_thread.detach();
            g_print("Now playing: %s\n", full_path.c_str());
        }
        else
        {
            g_print("No audio files found in the selected folder.\n");
        }

        for (size_t i = 0; i < song_list.size(); ++i)
            g_print("  [%zu] %s\n", i, song_list[i].c_str());

        g_free(folder_path_c);
    }

    gtk_widget_destroy(dialog);
}
