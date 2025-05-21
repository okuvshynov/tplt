#include "test_framework.hpp"
#include "heatmap_builder.hpp"
#include <tuple>
#include <vector>
#include <iostream>

// Test the map_range function
bool test_map_range() {
    // Test with integer types
    bool test1 = test::assert_equal(map_range(5, 0, 10, 0, 100), 50);
    
    // Test with different input/output types
    bool test2 = test::assert_equal(map_range(5, 0, 10, 0.0, 1.0), 0.5);
    
    // Test with boundary values
    bool test3 = test::assert_equal(map_range(0, 0, 10, 0, 100), 0);
    bool test4 = test::assert_equal(map_range(10, 0, 10, 0, 100), 100);
    
    // Test with negative numbers
    bool test5 = test::assert_equal(map_range(-5, -10, 0, 0, 100), 50);
    
    return test1 && test2 && test3 && test4 && test5;
}

// Test build_heatmap_data for 2D points
bool test_build_heatmap_2d() {
    // Create a set of 2D points
    std::vector<std::tuple<double, double>> points = {
        {0.0, 0.0}, {0.5, 0.5}, {1.0, 1.0},  // Diagonal points
        {0.0, 0.0}, {0.0, 0.0}               // Multiple points at same location
    };
    
    // Create a 3x3 heatmap
    auto heatmap = build_heatmap_data(points, 3, 3);
    
    // Expected heatmap - origin should have 3 points, middle should have 1, corner should have 1
    std::vector<std::vector<int>> expected = {
        {3, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    
    bool result = true;
    for (size_t y = 0; y < 3; y++) {
        for (size_t x = 0; x < 3; x++) {
            if (heatmap[y][x] != expected[y][x]) {
                std::cout << "Mismatch at [" << y << "][" << x << "]: "
                          << "Expected " << expected[y][x] << ", got " << heatmap[y][x] << "\n";
                result = false;
            }
        }
    }
    
    return result;
}

// Test build_heatmap_data for 3D points with count aggregation
bool test_build_heatmap_3d_count() {
    // Create a set of 3D points
    std::vector<std::tuple<double, double, double>> points = {
        {0.0, 0.0, 10.0}, {0.5, 0.5, 20.0}, {1.0, 1.0, 30.0},
        {0.0, 0.0, 40.0}  // Second point at same location
    };
    
    // Create a 3x3 heatmap with count aggregation
    auto heatmap = build_heatmap_data(points, AggregateFunc::Count, 3, 3);
    
    // Expected heatmap - origin should have 2 points, middle should have 1, corner should have 1
    std::vector<std::vector<double>> expected = {
        {2, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    
    bool result = true;
    for (size_t y = 0; y < 3; y++) {
        for (size_t x = 0; x < 3; x++) {
            if (heatmap[y][x] != expected[y][x]) {
                std::cout << "Mismatch at [" << y << "][" << x << "]: "
                          << "Expected " << expected[y][x] << ", got " << heatmap[y][x] << "\n";
                result = false;
            }
        }
    }
    
    return result;
}

// Test build_heatmap_data for 3D points with sum aggregation
bool test_build_heatmap_3d_sum() {
    // Create a set of 3D points
    std::vector<std::tuple<double, double, double>> points = {
        {0.0, 0.0, 10.0}, {0.5, 0.5, 20.0}, {1.0, 1.0, 30.0},
        {0.0, 0.0, 40.0}  // Second point at same location with different value
    };
    
    // Create a 3x3 heatmap with sum aggregation
    auto heatmap = build_heatmap_data(points, AggregateFunc::Sum, 3, 3);
    
    // Expected heatmap - origin should sum to 50, middle should be 20, corner should be 30
    std::vector<std::vector<double>> expected = {
        {50, 0, 0},
        {0, 20, 0},
        {0, 0, 30}
    };
    
    bool result = true;
    for (size_t y = 0; y < 3; y++) {
        for (size_t x = 0; x < 3; x++) {
            if (heatmap[y][x] != expected[y][x]) {
                std::cout << "Mismatch at [" << y << "][" << x << "]: "
                          << "Expected " << expected[y][x] << ", got " << heatmap[y][x] << "\n";
                result = false;
            }
        }
    }
    
    return result;
}

// Test build_heatmap_data for 3D points with avg aggregation
bool test_build_heatmap_3d_avg() {
    // Create a set of 3D points
    std::vector<std::tuple<double, double, double>> points = {
        {0.0, 0.0, 10.0}, {0.5, 0.5, 20.0}, {1.0, 1.0, 30.0},
        {0.0, 0.0, 40.0}  // Second point at same location with different value
    };
    
    // Create a 3x3 heatmap with average aggregation
    auto heatmap = build_heatmap_data(points, AggregateFunc::Avg, 3, 3);
    
    // Expected heatmap - origin should average to 25, middle should be 20, corner should be 30
    std::vector<std::vector<double>> expected = {
        {25, 0, 0},
        {0, 20, 0},
        {0, 0, 30}
    };
    
    bool result = true;
    for (size_t y = 0; y < 3; y++) {
        for (size_t x = 0; x < 3; x++) {
            if (!test::assert_equal(heatmap[y][x], expected[y][x])) {
                std::cout << "Mismatch at [" << y << "][" << x << "]: "
                          << "Expected " << expected[y][x] << ", got " << heatmap[y][x] << "\n";
                result = false;
            }
        }
    }
    
    return result;
}

// Main test function
int main() {
    test::TestSuite heatmap_builder_tests("HeatmapBuilder Tests");
    
    // Add test cases
    heatmap_builder_tests.add_test("map_range", test_map_range);
    heatmap_builder_tests.add_test("build_heatmap_2d", test_build_heatmap_2d);
    heatmap_builder_tests.add_test("build_heatmap_3d_count", test_build_heatmap_3d_count);
    heatmap_builder_tests.add_test("build_heatmap_3d_sum", test_build_heatmap_3d_sum);
    heatmap_builder_tests.add_test("build_heatmap_3d_avg", test_build_heatmap_3d_avg);
    
    // Run tests
    heatmap_builder_tests.run();
    
    // Return 0 if all tests passed, 1 otherwise
    return heatmap_builder_tests.all_passed() ? 0 : 1;
}