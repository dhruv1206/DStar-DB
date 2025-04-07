cmake_minimum_required(VERSION 3.16)

project(DStarDB)

set(CMAKE_CXX_STANDARD 17)

# Include your source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(dstardb ${SOURCES})

# Link with the ws2_32 library (Windows-specific)
target_link_libraries(dstardb PRIVATE ws2_32)
