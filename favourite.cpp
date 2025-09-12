#include "favourite.h"
#include <algorithm>
#include <iostream>
#include <fstream>   // for file I/O

std::vector<std::string> favourite_songs;

void on_FavButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *window = GTK_WIDGET(data);

    GtkWidget *songLabel = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
    const char *current_song = songLabel ? gtk_label_get_text(GTK_LABEL(songLabel)) : nullptr;

    if (!current_song || strlen(current_song) == 0 || strcmp(current_song, "No song playing yet") == 0) {
        g_print("Fav: no current song to toggle\n");
        return;
    }

    std::string song = current_song;

    bool isFavourite = std::find(favourite_songs.begin(), favourite_songs.end(), song) != favourite_songs.end();

    if (!isFavourite) {
        add_to_favourites(song);
        GtkWidget *image = gtk_image_new_from_icon_name("starred-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(widget), image);
        g_print("Fav: added '%s'\n", song.c_str());
    } else {
        remove_from_favourites(song);
        GtkWidget *image = gtk_image_new_from_icon_name("non-starred-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(widget), image);
        g_print("Fav: removed '%s'\n", song.c_str());
    }
}

void add_to_favourites(const std::string &song) {
    // prevent duplicates just in case
    if (std::find(favourite_songs.begin(), favourite_songs.end(), song) == favourite_songs.end()) {
        favourite_songs.push_back(song);
        save_favourites_to_file();   // save immediately
    }
    print_favourites_console();
}

void remove_from_favourites(const std::string &song) {
    favourite_songs.erase(std::remove(favourite_songs.begin(), favourite_songs.end(), song), favourite_songs.end());
    save_favourites_to_file();   // also save after removal
    print_favourites_console();
}

void print_favourites_console()
{
    if (favourite_songs.empty()) {
        std::cout << "No songs in favourites yet." << std::endl;
        return;
    }

    std::cout << "Favourite Songs:" << std::endl;
    for (size_t i = 0; i < favourite_songs.size(); ++i) {
        std::cout << i + 1 << ". " << favourite_songs[i] << " ❤️" << std::endl;
    }
}

// ===================================================
// Persistence helpers
// ===================================================
void save_favourites_to_file() {
    std::ofstream out("data/favourites.txt");
    if (!out) {
        std::cerr << "Error: could not open favourites file for writing" << std::endl;
        return;
    }

    for (const auto &song : favourite_songs) {
        out << song << "\n";
    }
}

void load_favourites_from_file() {
    favourite_songs.clear();
    std::ifstream in("data/favourites.txt");
    if (!in) {
        // file might not exist the first time — that's fine
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty())
            favourite_songs.push_back(line);
    }
}
