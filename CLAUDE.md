# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

TPLT is a C++ template library for generating heatmaps in the terminal. It provides functionality to:
- Create heatmaps from 2D and 3D data points
- Render heatmaps with different intensity levels using Unicode characters
- Support various aggregation functions (Sum, Average, Count)

## Building the Project

The project uses CMake as its build system:

```bash
# Create a build directory if it doesn't exist
mkdir -p build

# Configure the project
cd build
cmake ..

# Build the project
make

# Run the executable
./tplt
```

## Running Tests

The project includes a minimal testing framework and tests for the heatmap builder:

```bash
# Build the tests
cd build
cmake ..
make

# Run all tests
make test

# Or run a specific test executable directly
./heatmap_builder_test
```

## Code Architecture

The project is organized into multiple files:

1. **src/heatmap_builder.hpp**
   - Contains the `Numeric` concept for type constraints
   - Implements the `map_range` function for value mapping
   - Defines the `AggregateFunc` enum for aggregation methods
   - Provides `build_heatmap_data` template functions for generating heatmaps from 2D and 3D points

2. **src/heatmap_renderer.hpp**
   - Defines the `INTENSITY_CHARS` for visual representation
   - Implements the `get_intensity_char` function for intensity mapping
   - Provides the `render_heatmap` template function for visualization

3. **src/main.cpp**
   - Contains example usage of the library components

4. **src/test_framework.hpp**
   - A minimal unit testing framework
   - Provides TestSuite and TestCase classes
   - Includes assertion functions for writing tests

5. **src/heatmap_builder_test.cpp**
   - Unit tests for the heatmap builder functionality

## Core Components

1. **Numeric Type Concept** - Uses C++20 concepts to ensure template functions work with numeric types
2. **Data Processing Functions**:
   - `map_range` - Maps values from one range to another
   - `get_intensity_char` - Converts normalized values to visual intensity characters
3. **Heatmap Generation**:
   - `build_heatmap_data` - Constructs heatmaps from 2D or 3D points with optional aggregation
4. **Rendering**:
   - `render_heatmap` - Displays heatmaps in the terminal with optional legend
5. **Testing**:
   - Simple test framework with TestSuite and assertion functions

## Extending the Project

When adding new features:
1. For new aggregation functions, extend the `AggregateFunc` enum in `heatmap_builder.hpp`
2. For new visualization styles, modify the `INTENSITY_CHARS` vector in `heatmap_renderer.hpp`
3. For additional data processing capabilities, implement them as template functions in the appropriate header file
4. When adding new functionality, always add corresponding unit tests
5. Remember to update the `CMakeLists.txt` file if you add new source or header files