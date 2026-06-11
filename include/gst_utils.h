#pragma once

#include <memory>
#include <gst/gst.h>
using namespace std;

/**
 * @file gst_utils.h
 * @brief GStreamer RAII wrapper utilities for modern C++
 * 
 * Provides smart pointer deleters and helper utilities for managing
 * GStreamer resources automatically without manual cleanup.
 */

namespace gst {

/**
 * @brief Custom deleter for GStreamer objects
 * 
 * Automatically calls g_object_unref when the unique_ptr is destroyed.
 * Usage: unique_ptr<GstElement, gst::GstObjectDeleter> element(gst_element_factory_make(...));
 */
struct GstObjectDeleter {
    void operator()(gpointer ptr) const noexcept {
        if (ptr) {
            g_object_unref(ptr);
        }
    }
};

/**
 * @brief Specialized deleter for GStreamer caps
 * 
 * Automatically calls gst_caps_unref when destroyed.
 */
struct GstCapsDeleter {
    void operator()(GstCaps* ptr) const noexcept {
        if (ptr) {
            gst_caps_unref(ptr);
        }
    }
};

/**
 * @brief Specialized deleter for GStreamer pads
 * 
 * Automatically calls gst_object_unref when destroyed.
 */
struct GstPadDeleter {
    void operator()(GstPad* ptr) const noexcept {
        if (ptr) {
            gst_object_unref(ptr);
        }
    }
};

/**
 * @brief Specialized deleter for GStreamer messages
 * 
 * Automatically calls gst_message_unref when destroyed.
 */
struct GstMessageDeleter {
    void operator()(GstMessage* ptr) const noexcept {
        if (ptr) {
            gst_message_unref(ptr);
        }
    }
};

/**
 * @brief Specialized deleter for GStreamer bus
 * 
 * Automatically calls gst_object_unref when destroyed.
 */
struct GstBusDeleter {
    void operator()(GstBus* ptr) const noexcept {
        if (ptr) {
            gst_object_unref(ptr);
        }
    }
};

// Convenience type aliases for common GStreamer smart pointers
using GstElementPtr = unique_ptr<GstElement, GstObjectDeleter>;
using GstCapsPtr = unique_ptr<GstCaps, GstCapsDeleter>;
using GstPadPtr = unique_ptr<GstPad, GstPadDeleter>;
using GstMessagePtr = unique_ptr<GstMessage, GstMessageDeleter>;
using GstBusPtr = unique_ptr<GstBus, GstBusDeleter>;

/**
 * @brief Create a GStreamer element as a unique_ptr
 * @param factory The factory name (e.g., "filesrc")
 * @param name The element name
 * @return unique_ptr to GstElement or nullptr if creation failed
 */
inline GstElementPtr make_element(const char* factory, const char* name) {
    return GstElementPtr(gst_element_factory_make(factory, name));
}

/**
 * @brief Create a GStreamer bin as a unique_ptr
 * @param name The bin name
 * @return unique_ptr to GstElement (bin) or nullptr if creation failed
 */
inline GstElementPtr make_bin(const char* name) {
    return GstElementPtr(gst_bin_new(name));
}

/**
 * @brief Create a GStreamer pipeline as a unique_ptr
 * @param name The pipeline name
 * @return unique_ptr to GstElement (pipeline) or nullptr if creation failed
 */
inline GstElementPtr make_pipeline(const char* name) {
    return GstElementPtr(gst_pipeline_new(name));
}

} // namespace gst
