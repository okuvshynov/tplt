#pragma once

#include <vector>
#include <tuple>
#include <limits>
#include <algorithm>
#include <cmath>
#include <type_traits>

// Template concept for numeric types
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

// Maps a value from the input range to the output range
template<Numeric T, Numeric U>
U map_range(T value, T in_min, T in_max, U out_min, U out_max) {
    return static_cast<U>((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
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