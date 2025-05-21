cmake_minimum_required(VERSION 3.14)
project(tplt VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add header files
set(HEADERS
    src/heatmap_builder.hpp
    src/heatmap_renderer.hpp
)

# Add source files
set(SOURCES
    src/main.cpp
)

# Create the executable
add_executable(tplt ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(tplt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)