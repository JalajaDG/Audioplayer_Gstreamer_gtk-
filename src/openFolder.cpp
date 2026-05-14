#include "openFolder.h"
#include "player_state.h"
#include "PlayAudio.h"
#include <thread>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <sys/stat.h>

using namespace std;

static void handle_folder_change_and_play(const string &full_path, const string &new_folder_path, GtkWidget *window) noexcept;

static void show_error_dialog(GtkWidget *parent_window, const string &error_message)
{
    GtkWidget *error_dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "Error");

    gtk_message_dialog_format_secondary_text(
        GTK_MESSAGE_DIALOG(error_dialog),
        "%s", error_message.c_str());

    gtk_dialog_run(GTK_DIALOG(error_dialog));
    gtk_widget_destroy(error_dialog);
}

static bool is_audio_file(const string &filename)
{
    return g_str_has_suffix(filename.c_str(), ".mp3") ||
           g_str_has_suffix(filename.c_str(), ".wav") ||
           g_str_has_suffix(filename.c_str(), ".ogg");
}

static bool is_valid_folder(const string &folder_path)
{
    struct stat sb;
    if (stat(folder_path.c_str(), &sb) == -1) return false;
    return S_ISDIR(sb.st_mode);
}

static void handle_folder_change_and_play(const string &full_path, const string &new_folder_path, GtkWidget *window) noexcept
{
    {
        lock_guard<mutex> lock(player_state::state_mutex);
        if (player_state::folder_path != new_folder_path) {
            g_print("Folder changed -> updating folder path: %s\n", new_folder_path.c_str());
            player_state::folder_path = new_folder_path;
        }
    }
    PlayAudio::getInstance().play_audioFile(full_path, new_folder_path, window);
}

void open_default_folder(GtkWidget *window) noexcept
{
    if (!window) {
        cerr << "ERROR: Window is null in open_default_folder\n";
        return;
    }

    const string default_path = "/home/ee213082/Music";
    g_print("Opening default folder: %s\n", default_path.c_str());

    if (!is_valid_folder(default_path)) {
        cerr << "ERROR: Default folder not accessible: " << default_path << '\n';
        GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));
        if (label) gtk_label_set_text(GTK_LABEL(label), "Default folder not accessible");
        show_error_dialog(window, "Default music folder not found at: " + default_path +
                         "\nPlease select a folder using the folder button.");
        return;
    }

    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));
    if (label) gtk_label_set_text(GTK_LABEL(label), default_path.c_str());

    GDir *dir = g_dir_open(default_path.c_str(), 0, NULL);
    if (!dir) {
        cerr << "ERROR: Failed to open directory: " << default_path << '\n';
        show_error_dialog(window, "Cannot open folder: " + default_path);
        return;
    }

    string full_path;
    {
        lock_guard<mutex> lock(player_state::state_mutex);
        player_state::song_list.clear();

        const gchar *file_name;
        while ((file_name = g_dir_read_name(dir)) != NULL) {
            if (is_audio_file(file_name))
                player_state::song_list.push_back(file_name);
        }
        g_dir_close(dir);
        sort(player_state::song_list.begin(), player_state::song_list.end());

        if (!player_state::song_list.empty()) {
            player_state::current_song_index = 0;
            player_state::folder_path = default_path;
            full_path = player_state::folder_path + "/" + player_state::song_list[0];
        }
    }

    if (!full_path.empty()) {
        g_print("Playing first song from default folder: %s\n", full_path.c_str());
        string local_folder = default_path;
        thread play_thread([full_path, local_folder, window]() {
            handle_folder_change_and_play(full_path, local_folder, window);
        });
        play_thread.detach();
    } else {
        cerr << "WARNING: No audio files found in default folder: " << default_path << '\n';
        show_error_dialog(window, "No audio files found in: " + default_path);
    }
}

void on_openFolder_clicked(GtkWidget *widget, gpointer data) noexcept
{
    (void)widget;
    GtkWidget *window = GTK_WIDGET(data);
    if (!window) {
        cerr << "ERROR: Window pointer is null\n";
        return;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open Folder",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (!dialog) {
        show_error_dialog(window, "Failed to open folder selector dialog");
        return;
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT) {
        gtk_widget_destroy(dialog);
        return;
    }

    char *folder_path_c = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    gtk_widget_destroy(dialog);

    if (!folder_path_c) {
        cerr << "ERROR: No folder path selected\n";
        return;
    }

    const string selected_path = folder_path_c;
    g_free(folder_path_c);
    g_print("Selected folder: %s\n", selected_path.c_str());

    if (!is_valid_folder(selected_path)) {
        show_error_dialog(window, "Selected folder is not accessible:\n" + selected_path);
        return;
    }

    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "folder_label"));
    if (label) gtk_label_set_text(GTK_LABEL(label), selected_path.c_str());

    GDir *dir = g_dir_open(selected_path.c_str(), 0, NULL);
    if (!dir) {
        show_error_dialog(window, "Cannot open directory:\n" + selected_path);
        return;
    }

    string full_path;
    int audio_file_count = 0;
    {
        lock_guard<mutex> lock(player_state::state_mutex);
        player_state::song_list.clear();

        const gchar *file_name;
        while ((file_name = g_dir_read_name(dir)) != NULL) {
            if (is_audio_file(file_name)) {
                player_state::song_list.push_back(file_name);
                audio_file_count++;
            }
        }
        g_dir_close(dir);
        sort(player_state::song_list.begin(), player_state::song_list.end());

        if (!player_state::song_list.empty()) {
            player_state::current_song_index = 0;
            full_path = selected_path + "/" + player_state::song_list[0];
        }
    }

    if (!full_path.empty()) {
        g_print("Starting playback: %s (%d files loaded)\n", full_path.c_str(), audio_file_count);
        thread play_thread([full_path, selected_path, window]() {
            handle_folder_change_and_play(full_path, selected_path, window);
        });
        play_thread.detach();
    } else {
        show_error_dialog(window, "No audio files (.mp3, .wav, .ogg) found in:\n" + selected_path);
    }
}
