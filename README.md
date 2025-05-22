# tplt

A C++ template library for generating terminal heatmaps from stdin data.

## Features

- Read data from stdin with configurable delimiters
- CSV support with automatic header row detection (or explicit control)
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

# Use header column names (with auto-detection)
cat data.csv | ./tplt -d',' heatmap x_position y_position 'avg(intensity)'

# Force using header row
cat data.csv | ./tplt -d',' -header heatmap x_position y_position 'avg(intensity)'

# Force ignore header row
cat data.csv | ./tplt -d',' -no-header heatmap f1 f2
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

With a header row (automatically detected):

```
x_position,y_position,intensity,category
1.0,1.0,10.0,A
1.2,1.1,12.0,A
1.1,1.3,11.0,A
```

The program will automatically detect if the first row is a header (if it contains non-numeric values) and allow you to reference columns by name instead of index. You can control header detection with the following options:

- Auto-detection (default): The program will try to detect if the first row is a header
- `-header`: Force the first row to be treated as a header
- `-no-header`: Force the data to be treated as having no header

## Testing

```bash
# Build and run tests
cd build
cmake ..
make
make test

# Or run specific test executable directly
./heatmap_builder_test
./data_reader_test
```

## Project Structure

The project is organized into multiple files:

- **src/heatmap_builder.hpp**: Core data processing and heatmap generation
- **src/heatmap_renderer.hpp**: Terminal rendering and visualization
- **src/arg_parser.hpp**: Command-line argument parsing
- **src/data_reader.hpp**: Data reading from stdin with column selection and header detection
- **src/main.cpp**: Example usage and CLI interface
- **src/test_framework.hpp**: Minimal unit testing framework
- **src/heatmap_builder_test.cpp**: Tests for heatmap builder functionality
- **src/data_reader_test.cpp**: Tests for header detection and field name lookup

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
