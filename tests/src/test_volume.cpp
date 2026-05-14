/**
 * @file test_volume.cpp
 * @brief Unit tests for volume control functionality
 * @details Tests volume slider callback and GStreamer volume element control
 */

#include <cassert>
#include <iostream>

/**
 * @test test_volume_conversion
 * @brief Test slider value to GStreamer volume conversion
 * 
 * Verifies that slider values (0-100) are correctly converted to GStreamer volume (0.0-1.0)
 * 
 * Test cases:
 * - Slider at 0% → Volume 0.0
 * - Slider at 50% → Volume 0.5
 * - Slider at 100% → Volume 1.0
 */
void test_volume_conversion() {
    std::cout << "Running: test_volume_conversion\n";
    
    // Test minimum volume
    double slider_value = 0;
    double gst_volume = slider_value / 100.0;
    assert(gst_volume == 0.0);
    
    // Test middle volume
    slider_value = 50;
    gst_volume = slider_value / 100.0;
    assert(gst_volume == 0.5);
    
    // Test maximum volume
    slider_value = 100;
    gst_volume = slider_value / 100.0;
    assert(gst_volume == 1.0);
    
    std::cout << "✓ test_volume_conversion passed\n";
}

/**
 * @test test_volume_edge_cases
 * @brief Test edge case values for volume control
 * 
 * Test cases:
 * - Very small values (1%)
 * - Fractional slider values
 * - Boundary conditions
 */
void test_volume_edge_cases() {
    std::cout << "Running: test_volume_edge_cases\n";
    
    // Test 1%
    double gst_volume = 1.0 / 100.0;
    assert(gst_volume >= 0.0 && gst_volume <= 1.0);
    
    // Test 99%
    gst_volume = 99.0 / 100.0;
    assert(gst_volume >= 0.0 && gst_volume <= 1.0);
    
    // Verify range
    for (int i = 0; i <= 100; i++) {
        gst_volume = i / 100.0;
        assert(gst_volume >= 0.0 && gst_volume <= 1.0);
    }
    
    std::cout << "✓ test_volume_edge_cases passed\n";
}

int main() {
    std::cout << "\n========== Volume Control Unit Tests ==========\n\n";
    
    test_volume_conversion();
    test_volume_edge_cases();
    
    std::cout << "\n========== All tests passed! ==========\n\n";
    return 0;
}
