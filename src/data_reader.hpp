#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <optional>
#include <tuple>
#include <stdexcept>
#include "arg_parser.hpp"

namespace tplt {

// Container for a row of data
using DataRow = std::vector<std::string>;

// Data point with x, y and optional value
template<typename T = double>
struct DataPoint {
    T x;
    T y;
    std::optional<T> value;
    
    DataPoint(T x_val, T y_val) : x(x_val), y(y_val), value(std::nullopt) {}
    DataPoint(T x_val, T y_val, T val) : x(x_val), y(y_val), value(val) {}
};

class DataReader {
private:
    char delimiter_;
    
public:
    explicit DataReader(char delimiter = ' ') : delimiter_(delimiter) {}
    
    // Split a line into fields based on delimiter
    DataRow split_line(const std::string& line) const {
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        
        while (std::getline(ss, field, delimiter_)) {
            // Trim whitespace
            field.erase(0, field.find_first_not_of(" \t\r\n"));
            field.erase(field.find_last_not_of(" \t\r\n") + 1);
            
            if (!field.empty()) {
                fields.push_back(field);
            }
        }
        
        return fields;
    }
    
    // Get field value based on field spec
    std::string get_field_value(const DataRow& row, const FieldSpec& field_spec) const {
        if (field_spec.is_index) {
            int index = field_spec.index - 1;  // Convert to 0-based
            if (index < 0 || index >= static_cast<int>(row.size())) {
                throw std::runtime_error("Field index " + std::to_string(field_spec.index) + 
                                        " out of range (1-" + std::to_string(row.size()) + ")");
            }
            return row[index];
        } else {
            // For now, we don't support named fields in the data
            // We only support "f<number>" which is translated to an index in the constructor
            throw std::runtime_error("Named fields not supported in data: " + field_spec.name);
        }
    }
    
    // Read and parse data from stdin according to options
    template<typename T = double>
    std::vector<DataPoint<T>> read_data(const Options& options) {
        std::vector<DataPoint<T>> data_points;
        std::string line;
        
        // Read data from stdin
        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;
            
            try {
                DataRow row = split_line(line);
                if (row.empty()) continue;
                
                // Get x and y values
                std::string x_str = get_field_value(row, options.x_field);
                std::string y_str = get_field_value(row, options.y_field);
                
                T x_val = static_cast<T>(std::stod(x_str));
                T y_val = static_cast<T>(std::stod(y_str));
                
                // Check if we need a value for aggregation
                if (options.aggregation.function != AggregationSpec::Function::Count && 
                    options.aggregation.field.has_value()) {
                    
                    std::string val_str = get_field_value(row, *options.aggregation.field);
                    T val = static_cast<T>(std::stod(val_str));
                    data_points.emplace_back(x_val, y_val, val);
                } else {
                    data_points.emplace_back(x_val, y_val);
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Skipping line due to error: " << e.what() << std::endl;
                continue;
            }
        }
        
        return data_points;
    }
};

} // namespace tplt