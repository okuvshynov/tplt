# tplt

A C++ template library for generating terminal heatmaps from stdin data.

## Features

- Read data from stdin with configurable delimiters
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

## Usage

```bash
# Basic usage: read data from stdin and generate a heatmap
cat data.txt | ./tplt heatmap

# Specify a delimiter (comma in this case)
cat data.txt | ./tplt -d',' heatmap f1 f2

# Select specific columns (2nd and 4th)
cat data.txt | ./tplt heatmap f2 f4

# Use aggregation function (average of 7th column)
cat data.txt | ./tplt -d'|' heatmap f3 f5 'avg(f7)'
```

### Example run

```
% cat test_data/clustered_data_1k.csv | ./build/tplt -d',' heatmap f3 f4
░▓▓▒▒
▒▓█▓▓
 ░░░



              ░░░░░
              ▓▓█▓░
              ▒▓▒▒░


Legend:
  [0; 22)
░ [22; 44)
▒ [44; 67)
▓ [67; 89)
█ [89; 112)
```

### Data Format

The input data should be provided line by line from stdin, with each line representing a data point. For example:

```
1.2 3.4
2.5 5.6
3.7 7.8
```

With a custom delimiter:

```
1.2,3.4,10.0
2.5,5.6,20.0
3.7,7.8,30.0
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
- **src/arg_parser.hpp**: Command-line argument parsing
- **src/data_reader.hpp**: Data reading from stdin with column selection
- **src/main.cpp**: Example usage and CLI interface
- **src/test_framework.hpp**: Minimal unit testing framework
- **src/heatmap_builder_test.cpp**: Tests for heatmap builder functionality

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
