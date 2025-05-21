#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "heatmap_builder.hpp"
#include "heatmap_renderer.hpp"

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