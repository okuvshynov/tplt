#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <iomanip>
#include <tuple>
#include <functional>
#include <unordered_map>

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

// Enum for aggregation functions
enum class AggregateFunc {
    Sum,    // Sum of all values
    Avg,    // Average of all values
    Count   // Number of values (default)
};

// Build heatmap data from 2D points (x,y) or 3D points (x,y,v)
template<Numeric X, Numeric Y, Numeric V = int>
std::vector<std::vector<V>> build_heatmap_data(
    const std::vector<std::tuple<X, Y>>& points, 
    int width = 10, 
    int height = 10) {
    
    // Initialize empty heatmap with zeros
    std::vector<std::vector<V>> heatmap(height, std::vector<V>(width, 0));
    
    // Find min and max values for x and y to scale properly
    X min_x = std::numeric_limits<X>::max();
    X max_x = std::numeric_limits<X>::lowest();
    Y min_y = std::numeric_limits<Y>::max();
    Y max_y = std::numeric_limits<Y>::lowest();
    
    for (const auto& point : points) {
        X x = std::get<0>(point);
        Y y = std::get<1>(point);
        
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        min_y = std::min(min_y, y);
        max_y = std::max(max_y, y);
    }
    
    // Special case: all x or y values are the same
    if (min_x == max_x) max_x = min_x + 1;
    if (min_y == max_y) max_y = min_y + 1;
    
    // Count points in each cell
    for (const auto& point : points) {
        X x = std::get<0>(point);
        Y y = std::get<1>(point);
        
        // Map the point to the heatmap grid
        int cell_x = static_cast<int>(map_range(x, min_x, max_x, 0.0, static_cast<double>(width - 1)));
        int cell_y = static_cast<int>(map_range(y, min_y, max_y, 0.0, static_cast<double>(height - 1)));
        
        // Ensure we're within bounds
        cell_x = std::clamp(cell_x, 0, width - 1);
        cell_y = std::clamp(cell_y, 0, height - 1);
        
        // Increment the count for this cell
        heatmap[cell_y][cell_x]++;
    }
    
    return heatmap;
}

// Build heatmap data from 3D points (x,y,v) with optional aggregation function
template<Numeric X, Numeric Y, Numeric V>
std::vector<std::vector<V>> build_heatmap_data(
    const std::vector<std::tuple<X, Y, V>>& points, 
    AggregateFunc func = AggregateFunc::Count,
    int width = 10, 
    int height = 10) {
    
    // Initialize empty heatmap with zeros
    std::vector<std::vector<V>> heatmap(height, std::vector<V>(width, 0));
    
    // For average calculation, we need to track count separately
    std::vector<std::vector<int>> count_map(height, std::vector<int>(width, 0));
    
    // Find min and max values for x and y to scale properly
    X min_x = std::numeric_limits<X>::max();
    X max_x = std::numeric_limits<X>::lowest();
    Y min_y = std::numeric_limits<Y>::max();
    Y max_y = std::numeric_limits<Y>::lowest();
    
    for (const auto& point : points) {
        X x = std::get<0>(point);
        Y y = std::get<1>(point);
        
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        min_y = std::min(min_y, y);
        max_y = std::max(max_y, y);
    }
    
    // Special case: all x or y values are the same
    if (min_x == max_x) max_x = min_x + 1;
    if (min_y == max_y) max_y = min_y + 1;
    
    // Process points for each cell based on the aggregation function
    for (const auto& point : points) {
        X x = std::get<0>(point);
        Y y = std::get<1>(point);
        V v = std::get<2>(point);
        
        // Map the point to the heatmap grid
        int cell_x = static_cast<int>(map_range(x, min_x, max_x, 0.0, static_cast<double>(width - 1)));
        int cell_y = static_cast<int>(map_range(y, min_y, max_y, 0.0, static_cast<double>(height - 1)));
        
        // Ensure we're within bounds
        cell_x = std::clamp(cell_x, 0, width - 1);
        cell_y = std::clamp(cell_y, 0, height - 1);
        
        // Update the cell based on aggregation function
        if (func == AggregateFunc::Sum || func == AggregateFunc::Avg) {
            heatmap[cell_y][cell_x] += v;
            count_map[cell_y][cell_x]++;
        } else { // Count
            heatmap[cell_y][cell_x]++;
        }
    }
    
    // Post-process for average if needed
    if (func == AggregateFunc::Avg) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (count_map[y][x] > 0) {
                    heatmap[y][x] = static_cast<V>(static_cast<double>(heatmap[y][x]) / count_map[y][x]);
                }
            }
        }
    }
    
    return heatmap;
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

    // Example 3: Build heatmap from 2D points (x,y)
    std::vector<std::tuple<double, double>> points_2d = {
        {0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}, {0.7, 0.8}, {0.9, 1.0},
        {0.2, 0.3}, {0.4, 0.5}, {0.6, 0.7}, {0.8, 0.9}, {1.0, 0.1},
        {0.3, 0.4}, {0.5, 0.6}, {0.7, 0.8}, {0.9, 0.1}, {0.1, 0.2},
        {0.4, 0.5}, {0.6, 0.7}, {0.8, 0.9}, {0.1, 0.1}, {0.2, 0.3},
        // Add points clustered in a specific area to create a hotspot
        {0.2, 0.2}, {0.21, 0.21}, {0.22, 0.22}, {0.23, 0.23}, {0.24, 0.24},
        {0.25, 0.25}, {0.26, 0.26}, {0.27, 0.27}, {0.28, 0.28}, {0.29, 0.29}
    };
    
    // Create a 15x15 heatmap from 2D points
    auto heatmap_2d = build_heatmap_data(points_2d, 15, 15);
    
    std::cout << "\nExample 3: Heatmap from 2D points (count)\n";
    render_heatmap(heatmap_2d, true);
    
    // Example 4: Build heatmap from 3D points (x,y,v) with sum aggregation
    std::vector<std::tuple<double, double, double>> points_3d = {
        {0.1, 0.2, 5.0}, {0.3, 0.4, 2.5}, {0.5, 0.6, 7.8}, {0.7, 0.8, 1.2}, {0.9, 1.0, 3.4},
        {0.2, 0.3, 6.7}, {0.4, 0.5, 8.9}, {0.6, 0.7, 2.3}, {0.8, 0.9, 4.5}, {1.0, 0.1, 6.7},
        {0.3, 0.4, 8.9}, {0.5, 0.6, 1.2}, {0.7, 0.8, 3.4}, {0.9, 0.1, 5.6}, {0.1, 0.2, 7.8},
        {0.4, 0.5, 9.0}, {0.6, 0.7, 2.1}, {0.8, 0.9, 4.3}, {0.1, 0.1, 6.5}, {0.2, 0.3, 8.7},
        // Add points clustered in a specific area with high values
        {0.2, 0.2, 10.0}, {0.21, 0.21, 15.0}, {0.22, 0.22, 20.0}, {0.23, 0.23, 25.0}, {0.24, 0.24, 30.0}
    };
    
    // Create heatmaps with different aggregation functions
    auto heatmap_sum = build_heatmap_data(points_3d, AggregateFunc::Sum, 12, 12);
    auto heatmap_avg = build_heatmap_data(points_3d, AggregateFunc::Avg, 12, 12);
    auto heatmap_count = build_heatmap_data(points_3d, AggregateFunc::Count, 12, 12);
    
    std::cout << "\nExample 4a: Heatmap from 3D points (sum)\n";
    render_heatmap(heatmap_sum, true);
    
    std::cout << "\nExample 4b: Heatmap from 3D points (average)\n";
    render_heatmap(heatmap_avg, true);
    
    std::cout << "\nExample 4c: Heatmap from 3D points (count)\n";
    render_heatmap(heatmap_count, true);

    return 0;
}
