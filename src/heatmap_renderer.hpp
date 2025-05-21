#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include "heatmap_builder.hpp"

// Unicode characters for intensity levels
const std::vector<std::string> INTENSITY_CHARS = {" ", "░", "▒", "▓", "█"};

// Returns the intensity character based on normalized value (0-1)
std::string get_intensity_char(double normalized_value) {
    int index = static_cast<int>(normalized_value * (INTENSITY_CHARS.size() - 1));
    return INTENSITY_CHARS[index];
}

// Renders heatmap to the terminal
template<Numeric T>
void render_heatmap(const std::vector<std::vector<T>>& data, bool show_legend = false) {
    if (data.empty() || data[0].empty()) {
        std::cerr << "Error: Empty data provided\n";
        return;
    }

    // Find min and max values
    T min_val = data[0][0];
    T max_val = data[0][0];
    
    for (const auto& row : data) {
        for (const auto& val : row) {
            min_val = std::min(min_val, val);
            max_val = std::max(max_val, val);
        }
    }

    // Special case: all values are the same
    if (min_val == max_val) {
        min_val = min_val - 1;  // Arbitrary adjustment to avoid division by zero
    }

    // Render the heatmap
    for (const auto& row : data) {
        for (const auto& val : row) {
            double normalized = map_range(val, min_val, max_val, 0.0, 1.0);
            std::string intensity = get_intensity_char(normalized);
            
            std::cout << intensity;
        }
        std::cout << "\n";
    }

    // Render legend if requested
    if (show_legend) {
        std::cout << "\nLegend:\n";
        
        std::cout << std::fixed << std::setprecision(2);

        for (int i = 0; i < INTENSITY_CHARS.size(); ++i) {
            double mn = static_cast<double>(i)     / INTENSITY_CHARS.size();
            double mx = static_cast<double>(i + 1) / INTENSITY_CHARS.size();
            T mnd = map_range(mn, 0.0, 1.0, min_val, max_val);
            T mxd = map_range(mx, 0.0, 1.0, min_val, max_val);
            
            std::cout << INTENSITY_CHARS[i] << " [" << mn << "; " << mx << ")" << std::endl;
        }
    }
}