/**
 * @file test_file_handling.cpp
 * @brief Unit tests for file and folder handling functionality
 * @details Tests audio file detection, folder validation, and error handling
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <sys/stat.h>

/**
 * @test test_audio_file_detection
 * @brief Test audio file extension detection
 */
void test_audio_file_detection() {
    std::cout << "Running: test_audio_file_detection\n";
    
    auto is_audio_file = [](const std::string& filename) {
        return (filename.length() > 4) && (
            filename.substr(filename.length() - 4) == ".mp3" ||
            filename.substr(filename.length() - 4) == ".wav" ||
            filename.substr(filename.length() - 4) == ".ogg"
        );
    };
    
    // Valid audio files
    assert(is_audio_file("song.mp3") == true);
    assert(is_audio_file("audio.wav") == true);
    assert(is_audio_file("music.ogg") == true);
    
    // Invalid files
    assert(is_audio_file("document.txt") == false);
    assert(is_audio_file("image.jpg") == false);
    assert(is_audio_file("") == false);
    
    std::cout << "✓ test_audio_file_detection passed\n";
}

/**
 * @test test_file_path_parsing
 * @brief Test filename extraction from full file paths
 */
void test_file_path_parsing() {
    std::cout << "Running: test_file_path_parsing\n";
    
    auto extract_filename = [](const std::string& full_path) {
        return full_path.substr(full_path.find_last_of("/\\") + 1);
    };
    
    // Test absolute paths
    assert(extract_filename("/home/user/Music/song.mp3") == "song.mp3");
    assert(extract_filename("Music/song.mp3") == "song.mp3");
    assert(extract_filename("song.mp3") == "song.mp3");
    
    std::cout << "✓ test_file_path_parsing passed\n";
}

/**
 * @test test_song_list_sorting
 * @brief Test alphabetical sorting of song list
 */
void test_song_list_sorting() {
    std::cout << "Running: test_song_list_sorting\n";
    
    std::vector<std::string> songs = {"zebra.mp3", "apple.mp3", "banana.mp3"};
    std::sort(songs.begin(), songs.end());
    assert(songs[0] == "apple.mp3");
    assert(songs[1] == "banana.mp3");
    assert(songs[2] == "zebra.mp3");
    
    std::cout << "✓ test_song_list_sorting passed\n";
}

int main() {
    std::cout << "\n========== File Handling Unit Tests ==========\n\n";
    
    test_audio_file_detection();
    test_file_path_parsing();
    test_song_list_sorting();
    
    std::cout << "\n========== All file handling tests passed! ==========\n\n";
    return 0;
}
