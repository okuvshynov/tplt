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
    std::vector<std::string> headers_;
    bool has_headers_ = false;
    
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
            // Look up by field name
            if (!has_headers_) {
                throw std::runtime_error("Cannot use field name " + field_spec.name + 
                                      " when no header row was detected");
            }
            
            auto it = std::find(headers_.begin(), headers_.end(), field_spec.name);
            if (it == headers_.end()) {
                throw std::runtime_error("Field name not found in headers: " + field_spec.name);
            }
            
            int index = std::distance(headers_.begin(), it);
            if (index >= static_cast<int>(row.size())) {
                throw std::runtime_error("Field index for " + field_spec.name + 
                                       " out of range (index " + std::to_string(index) + 
                                       ", row size " + std::to_string(row.size()) + ")");
            }
            
            return row[index];
        }
    }
    
    // Check if a line is likely a header row
    bool is_likely_header(const DataRow& row) const {
        // Skip comment lines
        if (!row.empty() && row[0].length() > 0 && row[0][0] == '#') {
            return false;
        }
        
        // If we have at least 2 columns and none are convertible to numbers, it's likely a header
        if (row.size() < 2) return false;
        
        for (const auto& field : row) {
            try {
                std::stod(field);
                // If any field is convertible to a number, it's probably not a header
                return false;
            } catch (const std::exception&) {
                // Not a number, could be a header
            }
        }
        
        return true;
    }
    
    // Get headers (after read_data has been called)
    const std::vector<std::string>& get_headers() const {
        return headers_;
    }
    
    // Check if headers were detected
    bool has_headers() const {
        return has_headers_;
    }
    
    // Read and parse data from stdin according to options
    template<typename T = double>
    std::vector<DataPoint<T>> read_data(const Options& options) {
        std::vector<DataPoint<T>> data_points;
        std::string line;
        bool first_line = true;
        headers_.clear();
        has_headers_ = false;
        
        // Read data from stdin
        while (std::getline(std::cin, line)) {
            if (line.empty() || (line.length() > 0 && line[0] == '#')) continue;
            
            try {
                DataRow row = split_line(line);
                if (row.empty()) continue;
                
                // Check for header row on first non-empty line
                if (first_line) {
                    // Handle header based on options
                    if (options.header_mode == Options::HeaderMode::ForceOn) {
                        // Force using the first row as header
                        headers_ = row;
                        has_headers_ = true;
                        first_line = false;
                        continue;
                    } else if (options.header_mode == Options::HeaderMode::ForceOff) {
                        // Explicitly ignore header
                        has_headers_ = false;
                    } else if (is_likely_header(row)) {
                        // Auto-detect header (default behavior)
                        headers_ = row;
                        has_headers_ = true;
                        first_line = false;
                        continue;
                    }
                }
                
                first_line = false;
                
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