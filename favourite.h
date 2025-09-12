#ifndef FAVOURITE_H
#define FAVOURITE_H

#include <gtk/gtk.h>
#include <string>
#include <vector>

extern std::vector<std::string> favourite_songs;

void on_FavButton_clicked(GtkWidget *widget, gpointer data);
void add_to_favourites(const std::string &song);
void remove_from_favourites(const std::string &song);
bool is_favourite(const std::string &song);

void print_favourites_console();
void load_favourites_from_file();
void save_favourites_to_file();

#endif // FAVOURITE_H
