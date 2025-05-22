#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "heatmap_builder.hpp"
#include "heatmap_renderer.hpp"
#include "arg_parser.hpp"
#include "data_reader.hpp"

using namespace tplt;

// Convert DataPoint vector to the format needed by build_heatmap_data
template<typename T>
std::vector<std::tuple<T, T>> convert_to_2d_points(const std::vector<DataPoint<T>>& data_points) {
    std::vector<std::tuple<T, T>> points;
    
    for (const auto& point : data_points) {
        points.emplace_back(point.x, point.y);
    }
    
    return points;
}

// Convert DataPoint vector to the format needed by build_heatmap_data for 3D points
template<typename T>
std::vector<std::tuple<T, T, T>> convert_to_3d_points(const std::vector<DataPoint<T>>& data_points) {
    std::vector<std::tuple<T, T, T>> points;
    
    for (const auto& point : data_points) {
        if (point.value.has_value()) {
            points.emplace_back(point.x, point.y, point.value.value());
        } else {
            // If value is not provided, use 1.0 as default (for count)
            points.emplace_back(point.x, point.y, static_cast<T>(1.0));
        }
    }
    
    return points;
}

// Main function
int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        Options options = Options::parse(argc, argv);
        
        // Read data from stdin
        DataReader reader(options.delimiter);
        auto data_points = reader.read_data<double>(options);
        
        if (data_points.empty()) {
            std::cerr << "No valid data points were read." << std::endl;
            return 1;
        }
        
        // Inform about header detection
        if (reader.has_headers()) {
            std::string headerMode;
            switch (options.header_mode) {
                case Options::HeaderMode::Auto:
                    headerMode = "auto-detected";
                    break;
                case Options::HeaderMode::ForceOn:
                    headerMode = "enabled";
                    break;
                default:
                    headerMode = "detected";
            }
            
            std::cout << "Header row " << headerMode << ": ";
            const auto& headers = reader.get_headers();
            for (size_t i = 0; i < headers.size(); ++i) {
                std::cout << (i > 0 ? ", " : "") << headers[i];
            }
            std::cout << std::endl;
        } else if (options.header_mode == Options::HeaderMode::ForceOn) {
            std::cerr << "Warning: Header mode forced on, but no data was read" << std::endl;
        }

        // Process data based on command
        if (options.command == CommandType::Heatmap) {
            // Default heatmap dimensions
            const int width = 20;
            const int height = 10;

            // Check if we need aggregation
            if (options.aggregation.function == AggregationSpec::Function::Count && 
                !options.aggregation.field.has_value()) {
                // Simple 2D heatmap with count aggregation
                auto points_2d = convert_to_2d_points(data_points);
                auto heatmap = build_heatmap_data(points_2d, width, height);
                render_heatmap(heatmap, true);
            } else {
                // 3D heatmap with aggregation
                auto points_3d = convert_to_3d_points(data_points);
                
                // Map aggregation function
                AggregateFunc agg_func = AggregateFunc::Count;
                switch (options.aggregation.function) {
                    case AggregationSpec::Function::Sum:
                        agg_func = AggregateFunc::Sum;
                        break;
                    case AggregationSpec::Function::Avg:
                        agg_func = AggregateFunc::Avg;
                        break;
                    default:
                        agg_func = AggregateFunc::Count;
                        break;
                }
                
                auto heatmap = build_heatmap_data(points_3d, agg_func, width, height);
                render_heatmap(heatmap, true);
            }
        } else {
            std::cerr << "Unsupported command." << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Usage: tplt [options] command [fields]" << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  -d<char>      Set delimiter character" << std::endl;
        std::cerr << "  -header       Force first row to be treated as header" << std::endl;
        std::cerr << "  -no-header    Force data to be treated as having no header" << std::endl;
        std::cerr << "Examples:" << std::endl;
        std::cerr << "  cat data.txt | tplt -d',' heatmap f1 f2" << std::endl;
        std::cerr << "  cat data.txt | tplt heatmap f2 f4" << std::endl;
        std::cerr << "  cat data.txt | tplt -d'|' heatmap f3 f5 avg(f7)" << std::endl;
        std::cerr << "  cat data.csv | tplt -d',' -header heatmap xpos ypos avg(value)" << std::endl;
        return 1;
    }
    
    return 0;
}