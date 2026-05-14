#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <string_view>

/**
 * @class Favourites
 * @brief Manages user's favorite songs with persistent file storage
 */
class Favourites {
public:
    Favourites();
    ~Favourites() = default;
    
    // Prevent copying (favorites list is singleton-like)
    Favourites(const Favourites&) = delete;
    Favourites& operator=(const Favourites&) = delete;
    
    // Allow moving
    Favourites(Favourites&&) noexcept = default;
    Favourites& operator=(Favourites&&) noexcept = default;
    
    /// @brief Handle favorite button click event
    void onFavButtonClicked(GtkWidget *widget, gpointer data);
    
    /// @brief Add a song to favorites
    /// @param song Song path to add
    void add(std::string_view song);
    
    /// @brief Remove a song from favorites
    /// @param song Song path to remove
    void remove(std::string_view song);
    
    /// @brief Check if a song is in favorites
    /// @param song Song path to check
    /// @return true if song is a favorite
    [[nodiscard]] bool isFavourite(std::string_view song) const;
    
    /// @brief Print favorite songs to console
    void printConsole() const;
    
    /// @brief Load favorites from persistent storage
    void loadFromFile();
    
    /// @brief Save favorites to persistent storage
    void saveToFile() const;
    
    /// @brief Get all favorite songs
    /// @return const reference to the favorites list
    [[nodiscard]] const std::vector<std::string>& getFavourites() const noexcept {
        return favouriteSongs;
    }
    
private:
    std::vector<std::string> favouriteSongs;
};

