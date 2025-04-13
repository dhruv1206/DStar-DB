cmake_minimum_required(VERSION 3.15)
project(dstardb)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include headers)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
add_executable(dstardb ${SOURCES})

# Windows-specific libraries
if (WIN32)
    target_link_libraries(dstardb ws2_32)
endif()
