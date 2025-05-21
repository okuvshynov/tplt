# tplt

A C++ template library for generating terminal heatmaps.

## Features

- Generate heatmaps from 2D points (x,y) or 3D points (x,y,value)
- Multiple aggregation functions: Sum, Average, Count
- Render heatmaps using Unicode block characters with different intensity levels
- Show optional legends to interpret the visualization
- Includes a minimal testing framework

## Requirements

- C++20 compatible compiler
- CMake 3.14 or higher

## Building

```bash
# Create a build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make

# Run the examples
./tplt
```

## Testing

```bash
# Build and run tests
cd build
cmake ..
make
make test

# Or run specific test executable directly
./heatmap_builder_test
```

## Project Structure

The project is organized into multiple files:

- **src/heatmap_builder.hpp**: Core data processing and heatmap generation
- **src/heatmap_renderer.hpp**: Terminal rendering and visualization
- **src/main.cpp**: Example usage
- **src/test_framework.hpp**: Minimal unit testing framework
- **src/heatmap_builder_test.cpp**: Tests for heatmap builder functionality

## Usage

```cpp
#include "heatmap_builder.hpp"
#include "heatmap_renderer.hpp"
#include <vector>
#include <tuple>

// Example 1: Render a pre-defined heatmap
std::vector<std::vector<double>> data = {
    {0.0, 0.2, 0.4, 0.6, 0.8, 1.0},
    {0.1, 0.3, 0.5, 0.7, 0.9, 1.0},
    {0.2, 0.4, 0.6, 0.8, 1.0, 0.8}
};
render_heatmap(data, true);  // true enables legend

// Example 2: Generate a heatmap from 2D points
std::vector<std::tuple<double, double>> points_2d = {
    {0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}
};
auto heatmap = build_heatmap_data(points_2d, 10, 10);  // width=10, height=10
render_heatmap(heatmap);

// Example 3: Generate a heatmap from 3D points with aggregation
std::vector<std::tuple<double, double, double>> points_3d = {
    {0.1, 0.2, 5.0}, {0.3, 0.4, 2.5}, {0.5, 0.6, 7.8}
};
auto heatmap_sum = build_heatmap_data(points_3d, AggregateFunc::Sum, 12, 12);
render_heatmap(heatmap_sum);
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.