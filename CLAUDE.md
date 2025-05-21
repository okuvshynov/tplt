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

## Code Architecture

The project consists of several core components:

1. **Numeric Type Concept** - Uses C++20 concepts to ensure template functions work with numeric types
2. **Data Processing Functions**:
   - `map_range` - Maps values from one range to another
   - `get_intensity_char` - Converts normalized values to visual intensity characters
3. **Heatmap Generation**:
   - `build_heatmap_data` - Constructs heatmaps from 2D or 3D points with optional aggregation
4. **Rendering**:
   - `render_heatmap` - Displays heatmaps in the terminal with optional legend

## Extending the Project

When adding new features:
1. For new aggregation functions, extend the `AggregateFunc` enum
2. For new visualization styles, modify the `INTENSITY_CHARS` vector
3. For additional data processing capabilities, implement them as template functions