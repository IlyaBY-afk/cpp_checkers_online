cmake_minimum_required(VERSION 3.10)
project(CheckersGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform specific configurations
if(APPLE)
    # macOS specific settings
    set(SFML_INCLUDE_DIR "/usr/local/include")
    set(SFML_LIB_DIR "/usr/local/lib")
    
    # Include directories
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SFML_INCLUDE_DIR})
    link_directories(${SFML_LIB_DIR})
else()
    # For other platforms (Windows, Linux) use find_package
    find_package(SFML 3 COMPONENTS graphics window system audio network REQUIRED)
endif()

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(CheckersGame ${SOURCES})

# Link SFML libraries
if(APPLE)
    # On macOS, link to the dylib files directly
    target_link_libraries(CheckersGame 
        ${SFML_LIB_DIR}/libsfml-graphics.dylib
        ${SFML_LIB_DIR}/libsfml-window.dylib
        ${SFML_LIB_DIR}/libsfml-system.dylib
        ${SFML_LIB_DIR}/libsfml-audio.dylib
        ${SFML_LIB_DIR}/libsfml-network.dylib
    )
else()
    # On other platforms, use the targets defined by find_package
    target_link_libraries(CheckersGame 
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        sfml-network
    )
endif()

# Include directories
if(NOT APPLE)
    target_include_directories(CheckersGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 