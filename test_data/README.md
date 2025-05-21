# Test Data Files

This directory contains test data files for the TPLT application. These files can be used to test various functionality like different delimiters, column selection, and aggregation functions.

## Files

1. **space_delimited.txt**
   - Space-delimited data with X and Y coordinates
   - Simple 2D points forming distinctive clusters
   - Format: `X Y`

2. **comma_delimited.txt**
   - Comma-delimited data with X, Y, and Value columns
   - Data points with associated values for testing aggregation
   - Format: `X,Y,Value`

3. **pipe_delimited.txt**
   - Pipe-delimited data with multiple columns
   - Complex data for testing advanced column selection and aggregation
   - Format: `ID|Category|X|Y|Value1|Value2|Value3`

## Usage Examples

### Basic Usage (space-delimited)

```bash
cat test_data/space_delimited.txt | ./tplt heatmap
```

This will use the first two columns as X and Y coordinates with space delimiter.

### Comma-delimited with Value Column

```bash
# Using sum aggregation on the 3rd column
cat test_data/comma_delimited.txt | ./tplt -d',' heatmap f1 f2 sum(f3)

# Using average aggregation on the 3rd column
cat test_data/comma_delimited.txt | ./tplt -d',' heatmap f1 f2 avg(f3)
```

### Pipe-delimited with Column Selection

```bash
# Using columns 3 and 4 as X and Y
cat test_data/pipe_delimited.txt | ./tplt -d'|' heatmap f3 f4

# Using columns 3 and 4 as X and Y with sum of column 5
cat test_data/pipe_delimited.txt | ./tplt -d'|' heatmap f3 f4 sum(f5)

# Using columns 3 and 4 as X and Y with average of column 6
cat test_data/pipe_delimited.txt | ./tplt -d'|' heatmap f3 f4 avg(f6)

# Using columns 3 and 4 as X and Y with average of column 7
cat test_data/pipe_delimited.txt | ./tplt -d'|' heatmap f3 f4 avg(f7)
```

These examples demonstrate how to use the test data files with different command line options.