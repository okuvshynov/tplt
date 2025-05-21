#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <iomanip>

// Template concept for numeric types
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

// Unicode characters for intensity levels
const std::vector<std::string> INTENSITY_CHARS = {" ", "░", "▒", "▓", "█"};

// Maps a value from the input range to the output range
template<Numeric T, Numeric U>
U map_range(T value, T in_min, T in_max, U out_min, U out_max) {
    return static_cast<U>((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

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

// Example usage
int main() {
    // Example 1: Simple gradient
    std::vector<std::vector<double>> example1 = {
        {0.0, 0.2, 0.4, 0.6, 0.8, 1.0},
        {0.1, 0.3, 0.5, 0.7, 0.9, 1.0},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.8},
        {0.3, 0.5, 0.7, 0.9, 0.7, 0.5},
        {0.4, 0.6, 0.8, 0.6, 0.4, 0.2},
        {0.5, 0.7, 0.5, 0.3, 0.1, 0.0}
    };

    std::cout << "Example 1: Simple gradient\n";
    render_heatmap(example1, true);
    
    // Example 2: Random-like pattern with integers
    std::vector<std::vector<int>> example2 = {
        {5, 10, 15, 20, 25, 30, 35, 40},
        {40, 35, 30, 25, 20, 15, 10, 5},
        {10, 20, 30, 40, 35, 25, 15, 5},
        {5, 15, 25, 35, 40, 30, 20, 10},
        {20, 10, 5, 15, 25, 35, 40, 30},
        {30, 40, 35, 25, 15, 5, 10, 20}
    };
    
    std::cout << "\nExample 2: Random-like pattern\n";
    render_heatmap(example2, true);

    return 0;
}
