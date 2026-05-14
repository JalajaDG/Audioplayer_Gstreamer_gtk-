/**
 * @file test_playlist.cpp
 * @brief Unit tests for playlist navigation functionality
 * @details Tests play next, play previous, and playlist index management
 */

#include <iostream>
#include <vector>
#include <cassert>

/**
 * Mock playlist for testing
 */
class MockPlaylist {
public:
    std::vector<std::string> songs;
    int currently_playing_index;
    
    MockPlaylist() : currently_playing_index(-1) {}
    
    void add_song(const std::string& song) {
        songs.push_back(song);
    }
    
    void clear() {
        songs.clear();
        currently_playing_index = -1;
    }
    
    bool play_next() {
        if (songs.empty()) return false;
        
        if (currently_playing_index < 0) {
            currently_playing_index = 0;
        } else if (currently_playing_index < (int)songs.size() - 1) {
            currently_playing_index++;
        } else {
            currently_playing_index = 0;
        }
        return true;
    }
    
    bool play_previous() {
        if (songs.empty()) return false;
        
        if (currently_playing_index <= 0) {
            currently_playing_index = songs.size() - 1;
        } else {
            currently_playing_index--;
        }
        return true;
    }
    
    std::string get_current_song() {
        if (currently_playing_index >= 0 && currently_playing_index < (int)songs.size()) {
            return songs[currently_playing_index];
        }
        return "";
    }
};

/**
 * @test test_play_next
 * @brief Test play next functionality
 */
void test_play_next() {
    std::cout << "Running: test_play_next\n";
    
    MockPlaylist playlist;
    
    // Test with empty playlist
    assert(playlist.play_next() == false);
    
    // Add songs
    playlist.add_song("song1.mp3");
    playlist.add_song("song2.mp3");
    playlist.add_song("song3.mp3");
    
    // Test: First song
    assert(playlist.play_next() == true);
    assert(playlist.currently_playing_index == 0);
    assert(playlist.get_current_song() == "song1.mp3");
    
    // Test: Move to second song
    assert(playlist.play_next() == true);
    assert(playlist.currently_playing_index == 1);
    assert(playlist.get_current_song() == "song2.mp3");
    
    // Test: Wrap around to first song
    assert(playlist.play_next() == true);
    assert(playlist.play_next() == true);
    assert(playlist.currently_playing_index == 0);
    
    std::cout << "✓ test_play_next passed\n";
}

/**
 * @test test_play_previous
 * @brief Test play previous functionality
 */
void test_play_previous() {
    std::cout << "Running: test_play_previous\n";
    
    MockPlaylist playlist;
    
    // Test with empty playlist
    assert(playlist.play_previous() == false);
    
    // Add songs
    playlist.add_song("song1.mp3");
    playlist.add_song("song2.mp3");
    playlist.add_song("song3.mp3");
    
    // Start at song 2
    playlist.currently_playing_index = 1;
    
    // Move to first song
    assert(playlist.play_previous() == true);
    assert(playlist.currently_playing_index == 0);
    
    // Wrap around to last song
    assert(playlist.play_previous() == true);
    assert(playlist.currently_playing_index == 2);
    
    std::cout << "✓ test_play_previous passed\n";
}

int main() {
    std::cout << "\n========== Playlist Navigation Unit Tests ==========\n\n";
    
    test_play_next();
    test_play_previous();
    
    std::cout << "\n========== All playlist tests passed! ==========\n\n";
    return 0;
}
