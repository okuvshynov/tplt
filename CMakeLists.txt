cmake_minimum_required(VERSION 3.14)
project(tplt VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add header files
set(HEADERS
    src/heatmap_builder.hpp
    src/heatmap_renderer.hpp
    src/test_framework.hpp
    src/arg_parser.hpp
    src/data_reader.hpp
)

# Add source files for main executable
set(SOURCES
    src/main.cpp
)

# Create the main executable
add_executable(tplt ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(tplt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add test executables
add_executable(heatmap_builder_test src/heatmap_builder_test.cpp ${HEADERS})
target_include_directories(heatmap_builder_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(data_reader_test src/data_reader_test.cpp ${HEADERS})
target_include_directories(data_reader_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add custom target to run all tests
add_custom_target(test 
    COMMAND heatmap_builder_test
    COMMAND data_reader_test
    DEPENDS heatmap_builder_test data_reader_test
    COMMENT "Running tests..."
)