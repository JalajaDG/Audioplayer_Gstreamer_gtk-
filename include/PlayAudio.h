#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/gstelement.h>
#include <gst/audio/audio.h>
#include <gst/video/video.h>
#include <gst/video/videooverlay.h>
#include "gst_utils.h"

/**
 * @class PlayAudio
 * @brief Core audio player class implementing singleton pattern with modern C++
 * 
 * Manages GStreamer pipeline for audio playback with visualization support.
 * Implements Singleton design pattern to ensure only one player instance.
 * Uses smart pointers for automatic resource management (RAII).
 * 
 * Handles:
 * - Audio playback from files
 * - Volume control
 * - Seek/pause operations
 * - Audio visualization (Goom)
 * - GStreamer event handling
 */
class PlayAudio
{
private:
    // Singleton instance using unique_ptr for automatic cleanup
    static std::unique_ptr<PlayAudio> instance;
    
    // === GStreamer pipeline elements ===
    GstElement* pipeline{nullptr};           // Main pipeline (not managed via unique_ptr as it's unrefed in destructor)
    GstElement* filesrc{nullptr};            // File source element
    GstElement* decodebin{nullptr};          // Decoder element
    GstElement* tee{nullptr};                // Tee element for splitting audio/video
    
    GstElement* queue_audio{nullptr};        // Audio queue
    GstElement* audiobin{nullptr};           // Audio bin
    GstElement* queue_visualizer{nullptr};   // Visualizer queue
    GstElement* visualizer_bin{nullptr};     // Visualizer bin
    
    GstElement* videobin{nullptr};           // Video bin
    GstElement* queue_video_sink{nullptr};   // Video sink queue
    GstElement* videoconvert{nullptr};       // Video converter
    GstElement* gtksink{nullptr};            // GTK sink for visualization
    GstElement* goom{nullptr};               // Goom visualizer
    GstElement* volume_element{nullptr};     // Volume control element
    
    GstBus* bus{nullptr};                    // Message bus (not managed via unique_ptr)
    GstMessage* msg{nullptr};                // Current message
    
    GtkWidget* main_window{nullptr};         // Reference to main GTK window (not owned)
    
    // Private constructor for singleton pattern
    PlayAudio();
    
    // Prevent copy operations
    PlayAudio(const PlayAudio&) = delete;
    PlayAudio& operator=(const PlayAudio&) = delete;
    
    // Prevent move operations (singleton should not be moved)
    PlayAudio(PlayAudio&&) = delete;
    PlayAudio& operator=(PlayAudio&&) = delete;
    
    // Private destructor — friended so unique_ptr can call it
    ~PlayAudio();
    friend std::default_delete<PlayAudio>;

public:
    /**
     * @brief Get singleton instance of PlayAudio
     * @return Reference to the single PlayAudio instance
     */
    static PlayAudio& getInstance();
    
    /**
     * @brief Check if audio is currently playing
     * @return true if pipeline is in PLAYING state, false otherwise
     */
    bool isPlaying() const;

    /**
     * @brief Open folder and start playing first audio file
     * @param folder_path Path to the folder containing audio files
     */
    void openFolderAndPlayFirstSong(std::string_view folder_path);
    
    /**
     * @brief Get the GStreamer pipeline
     * @return Pointer to the main audio pipeline
     */
    GstElement* getPipeline() { return pipeline; }

    /**
     * @brief Play an audio file from specified path
     * @param file_path Path to the audio file to play
     * @param folder_path Path to the folder containing the file
     * @param window GTK window widget for displaying messages
     */
    void play_audioFile(std::string_view file_path, std::string_view folder_path, GtkWidget* window);
    
    /**
     * @brief Callback for GStreamer pad-added signal (dynamic pad linking)
     * @param element The GStreamer element that added the pad
     * @param pad The newly added pad
     * @param user_data User data pointer passed to the signal
     */
    static void on_pad_added(GstElement* element, GstPad* pad, gpointer user_data);
    
    /**
     * @brief Play next song in playlist
     */
    void play_next();
    
    /**
     * @brief GStreamer message bus callback handler
     * @param bus The message bus
     * @param msg The message from the bus
     * @param user_data User data pointer
     * @return TRUE to remove message from queue, FALSE to keep it
     */
    static gboolean on_message(GstBus*, GstMessage*, gpointer);
    
    /**
     * @brief Handle End-Of-Stream event (song finished playing)
     * @param window GTK window widget for displaying messages
     */
    void handleEOS(GtkWidget* window);

    /**
     * @brief Set the main GTK window reference
     * @param window Pointer to main application window
     */
    void setMainWindow(GtkWidget* window) noexcept { main_window = window; }
    
    /**
     * @brief Get the main GTK window reference
     * @return Pointer to main application window
     */
    [[nodiscard]] GtkWidget* getMainWindow() const noexcept { return main_window; }
    
    /**
     * @brief Stop the audio playback pipeline
     */
    void stopPipeline();
    
    /**
     * @brief Get the volume control element
     * @return Pointer to GStreamer volume element (used for mute/unmute)
     */
    [[nodiscard]] GstElement* getVolumeElement() noexcept { return volume_element; }
    
    /**
     * @brief Get the Goom visualization element
     * @return Pointer to Goom visualizer element
     */
    [[nodiscard]] GstElement* getGoomElement() noexcept { return goom; }
    
    /**
     * @brief Get the GTK sink for displaying visualizations
     * @return Pointer to GTK sink element
     */
    [[nodiscard]] GstElement* getGtkSink() noexcept { return gtksink; }
};
