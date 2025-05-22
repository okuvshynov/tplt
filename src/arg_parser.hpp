#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <iostream>
#include <regex>

namespace tplt {

// Supported command types
enum class CommandType {
    Heatmap,    // Generate a heatmap
    Unknown     // Unknown command
};

// Input field specifier
struct FieldSpec {
    bool is_index = false;  // True if using numeric index, false if using column name
    int index = 0;          // Field index (1-based)
    std::string name;       // Field name
    
    // Constructors
    FieldSpec() = default;
    
    explicit FieldSpec(int idx) : is_index(true), index(idx) {}
    
    explicit FieldSpec(const std::string& n) : is_index(false), name(n) {
        // If the name is in the format "f<number>", interpret as an index
        std::regex field_regex("f(\\d+)");
        std::smatch match;
        if (std::regex_match(n, match, field_regex)) {
            is_index = true;
            index = std::stoi(match[1].str());
        }
        // Otherwise, keep it as a field name for lookup in header row
    }
};

// Aggregation function with field
struct AggregationSpec {
    enum class Function {
        Count,  // Count occurrences (default)
        Sum,    // Sum values
        Avg     // Average values
    };
    
    Function function = Function::Count;
    std::optional<FieldSpec> field;  // Empty for count, populated for sum/avg
    
    // Constructors
    AggregationSpec() = default;
    
    static AggregationSpec parse(const std::string& spec) {
        AggregationSpec result;
        
        // Check if we have a function specification
        std::regex func_regex("(count|sum|avg)\\((.+)\\)", std::regex::icase);
        std::smatch match;
        
        if (std::regex_match(spec, match, func_regex)) {
            // We have a function specification with field
            std::string func = match[1].str();
            std::string field = match[2].str();
            
            std::transform(func.begin(), func.end(), func.begin(), ::tolower);
            
            if (func == "sum") {
                result.function = Function::Sum;
            } else if (func == "avg") {
                result.function = Function::Avg;
            } else {
                result.function = Function::Count;
            }
            
            result.field = FieldSpec(field);
        } else {
            // No function, just a field - use default count
            result.field = FieldSpec(spec);
        }
        
        return result;
    }
};

// Program options
struct Options {
    CommandType command = CommandType::Unknown;
    char delimiter = ' ';
    FieldSpec x_field;
    FieldSpec y_field;
    AggregationSpec aggregation;
    
    enum class HeaderMode {
        Auto,       // Automatically detect header (default)
        ForceOn,    // Force header row to be used
        ForceOff    // Force no header row
    };
    
    HeaderMode header_mode = HeaderMode::Auto;
    
    // Parse command line arguments
    static Options parse(int argc, char* argv[]) {
        Options opts;
        
        // Need at least a command
        if (argc < 2) {
            throw std::runtime_error("Not enough arguments. Usage: tplt [options] command [fields]");
        }
        
        int arg_index = 1;
        
        // Parse options starting with '-'
        while (arg_index < argc && argv[arg_index][0] == '-') {
            std::string arg = argv[arg_index];
            
            if (arg.substr(0, 2) == "-d") {
                if (arg.length() > 2) {
                    // Format: -d<delimiter>
                    opts.delimiter = arg[2];
                } else if (arg_index + 1 < argc) {
                    // Format: -d <delimiter>
                    opts.delimiter = argv[arg_index + 1][0];
                    arg_index++;
                } else {
                    throw std::runtime_error("Missing delimiter after -d");
                }
            } else if (arg == "-header") {
                opts.header_mode = HeaderMode::ForceOn;
            } else if (arg == "-no-header") {
                opts.header_mode = HeaderMode::ForceOff;
            } else {
                throw std::runtime_error("Unknown option: " + arg);
            }
            
            arg_index++;
        }
        
        // Parse command
        if (arg_index < argc) {
            std::string cmd = argv[arg_index++];
            if (cmd == "heatmap") {
                opts.command = CommandType::Heatmap;
            } else {
                throw std::runtime_error("Unknown command: " + cmd);
            }
        } else {
            throw std::runtime_error("No command specified");
        }
        
        // Parse fields based on command
        if (opts.command == CommandType::Heatmap) {
            // Heatmap needs x and y fields, with optional aggregation
            if (arg_index < argc) {
                opts.x_field = FieldSpec(argv[arg_index++]);
            } else {
                // Default to field 1 if not specified
                opts.x_field = FieldSpec(1);
            }
            
            if (arg_index < argc) {
                // Check if second arg is an aggregation function
                std::string arg = argv[arg_index];
                if (arg.find('(') != std::string::npos && arg.find(')') != std::string::npos) {
                    // This is an aggregation function
                    opts.aggregation = AggregationSpec::parse(arg);
                    opts.y_field = FieldSpec(2);  // Default to field 2 for y
                } else {
                    // This is the y field
                    opts.y_field = FieldSpec(arg);
                    arg_index++;
                    
                    // Check if there's an aggregation function
                    if (arg_index < argc) {
                        opts.aggregation = AggregationSpec::parse(argv[arg_index]);
                    }
                }
            } else {
                // Default to field 2 if not specified
                opts.y_field = FieldSpec(2);
            }
        }
        
        return opts;
    }
    
    void print() const {
        std::cout << "Command: " << (command == CommandType::Heatmap ? "heatmap" : "unknown") << std::endl;
        std::cout << "Delimiter: '" << delimiter << "'" << std::endl;
        
        std::cout << "Header mode: ";
        switch (header_mode) {
            case HeaderMode::Auto:
                std::cout << "auto-detect" << std::endl;
                break;
            case HeaderMode::ForceOn:
                std::cout << "forced on" << std::endl;
                break;
            case HeaderMode::ForceOff:
                std::cout << "forced off" << std::endl;
                break;
        }
        
        std::cout << "X field: ";
        if (x_field.is_index) {
            std::cout << "index " << x_field.index << std::endl;
        } else {
            std::cout << "name " << x_field.name << std::endl;
        }
        
        std::cout << "Y field: ";
        if (y_field.is_index) {
            std::cout << "index " << y_field.index << std::endl;
        } else {
            std::cout << "name " << y_field.name << std::endl;
        }
        
        std::cout << "Aggregation: ";
        switch (aggregation.function) {
            case AggregationSpec::Function::Count:
                std::cout << "count";
                break;
            case AggregationSpec::Function::Sum:
                std::cout << "sum";
                break;
            case AggregationSpec::Function::Avg:
                std::cout << "avg";
                break;
        }
        
        if (aggregation.field.has_value()) {
            std::cout << " of ";
            if (aggregation.field->is_index) {
                std::cout << "index " << aggregation.field->index;
            } else {
                std::cout << "name " << aggregation.field->name;
            }
        }
        
        std::cout << std::endl;
    }
};

} // namespace tplt