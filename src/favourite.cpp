// Refactored: Favourites class implementation with modern C++
#include "favourite.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>

Favourites::Favourites() {
    loadFromFile();
}

void Favourites::onFavButtonClicked(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);
    GtkWidget *songLabel = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "songLabel"));
    const char *current_song = songLabel ? gtk_label_get_text(GTK_LABEL(songLabel)) : nullptr;
    
    if (!current_song || std::strlen(current_song) == 0 || std::strcmp(current_song, "No song playing yet") == 0) {
        g_print("Fav: no current song to toggle\n");
        return;
    }
    
    std::string song{current_song};
    const bool isFav = isFavourite(song);
    
    if (!isFav) {
        add(song);
        GtkWidget *image = gtk_image_new_from_icon_name("starred-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(widget), image);
        g_print("Fav: added '%s'\n", song.c_str());
    } else {
        remove(song);
        GtkWidget *image = gtk_image_new_from_icon_name("non-starred-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(widget), image);
        g_print("Fav: removed '%s'\n", song.c_str());
    }
}

void Favourites::add(std::string_view song) {
    if (!isFavourite(song)) {
        favouriteSongs.emplace_back(song);
        saveToFile();
    }
    printConsole();
}

void Favourites::remove(std::string_view song) {
    auto it = std::find(favouriteSongs.begin(), favouriteSongs.end(), song);
    if (it != favouriteSongs.end()) {
        favouriteSongs.erase(it);
        saveToFile();
    }
    printConsole();
}

bool Favourites::isFavourite(std::string_view song) const {
    return std::find(favouriteSongs.begin(), favouriteSongs.end(), song) != favouriteSongs.end();
}

void Favourites::printConsole() const {
    if (favouriteSongs.empty()) {
        std::cout << "No songs in favourites yet.\n";
        return;
    }
    
    std::cout << "Favourite Songs:\n";
    for (size_t i = 0; i < favouriteSongs.size(); ++i) {
        std::cout << (i + 1) << ". " << favouriteSongs[i] << " ❤️\n";
    }
}

void Favourites::loadFromFile() {
    favouriteSongs.clear();
    std::ifstream fin("data/favourites.txt");
    if (!fin.is_open()) {
        g_print("Favourites file not found, starting fresh\n");
        return;
    }
    
    std::string line;
    while (std::getline(fin, line)) {
        if (!line.empty()) {
            favouriteSongs.push_back(line);
        }
    }
}

void Favourites::saveToFile() const {
    std::ofstream fout("data/favourites.txt");
    if (!fout.is_open()) {
        std::cerr << "Error: could not open favourites file for writing\n";
        return;
    }

    for (const auto &song : favouriteSongs) {
        fout << song << '\n';
    }
}
