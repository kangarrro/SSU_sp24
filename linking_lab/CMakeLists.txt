cmake_minimum_required(VERSION 3.10)
project(linking_lab)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Add subdirectory for LevelDB
add_subdirectory(leveldb)

# Include LevelDB headers
include_directories(leveldb/include)

# Add the main executable
add_executable(main main.cpp)

# Link LevelDB library to main executable
target_link_libraries(main PRIVATE leveldb)

# Set debugging options
target_compile_options(main PRIVATE -g)
target_link_options(main PRIVATE -g)

# Ensure debug symbols are generated
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    