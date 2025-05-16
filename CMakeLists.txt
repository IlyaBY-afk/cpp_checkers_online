cmake_minimum_required(VERSION 3.10)
project(CheckersGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
# Option 1: Use CMake's find_package
find_package(SFML 2 COMPONENTS graphics window system audio network REQUIRED)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(CheckersGame ${SOURCES})

# Link SFML libraries
target_link_libraries(CheckersGame 
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    sfml-network
)

# Include directories
target_include_directories(CheckersGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 