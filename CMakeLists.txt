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
elseif(WIN32)
    # Windows specific settings
    if(NOT DEFINED SFML_DIR)
        message(FATAL_ERROR "SFML_DIR not set. Please run cmake with -DSFML_DIR=<path_to_SFML>/lib/cmake/SFML")
    endif()
    
    # Get SFML root from SFML_DIR (which should be <SFML_ROOT>/lib/cmake/SFML)
    get_filename_component(SFML_ROOT "${SFML_DIR}" DIRECTORY)
    get_filename_component(SFML_ROOT "${SFML_ROOT}" DIRECTORY)
    
    set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
    set(SFML_LIB_DIR "${SFML_ROOT}/lib")
    
    # Debug output
    message(STATUS "SFML_ROOT: ${SFML_ROOT}")
    message(STATUS "SFML_LIB_DIR: ${SFML_LIB_DIR}")
    
    # Check if library files exist
    if(EXISTS "${SFML_LIB_DIR}/sfml-graphics.lib")
        message(STATUS "sfml-graphics.lib found at ${SFML_LIB_DIR}/sfml-graphics.lib")
    else()
        message(WARNING "sfml-graphics.lib NOT found at ${SFML_LIB_DIR}/sfml-graphics.lib")
        # List all files in the lib directory for debugging
        file(GLOB LIB_FILES "${SFML_LIB_DIR}/*.lib")
        message(STATUS "Available .lib files: ${LIB_FILES}")
    endif()
    
    # Include directories
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SFML_INCLUDE_DIR})
    link_directories(${SFML_LIB_DIR})
else()
    # For other platforms (Linux) use find_package
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
elseif(WIN32)
    # On Windows, link to the appropriate libraries with full paths
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(CheckersGame 
            ${SFML_LIB_DIR}/sfml-graphics-d.lib
            ${SFML_LIB_DIR}/sfml-window-d.lib
            ${SFML_LIB_DIR}/sfml-system-d.lib
            ${SFML_LIB_DIR}/sfml-audio-d.lib
            ${SFML_LIB_DIR}/sfml-network-d.lib
        )
    else()
        # For Visual Studio, we need a different approach since CMAKE_BUILD_TYPE is not used
        # Visual Studio uses a per-target configuration approach
        target_link_libraries(CheckersGame 
            optimized ${SFML_LIB_DIR}/sfml-graphics.lib
            optimized ${SFML_LIB_DIR}/sfml-window.lib
            optimized ${SFML_LIB_DIR}/sfml-system.lib
            optimized ${SFML_LIB_DIR}/sfml-audio.lib
            optimized ${SFML_LIB_DIR}/sfml-network.lib
            debug ${SFML_LIB_DIR}/sfml-graphics-d.lib
            debug ${SFML_LIB_DIR}/sfml-window-d.lib
            debug ${SFML_LIB_DIR}/sfml-system-d.lib
            debug ${SFML_LIB_DIR}/sfml-audio-d.lib
            debug ${SFML_LIB_DIR}/sfml-network-d.lib
        )
    endif()
    
    # Copy DLLs to build directory
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SFML_DLLS
            ${SFML_ROOT}/bin/sfml-graphics-d-3.dll
            ${SFML_ROOT}/bin/sfml-window-d-3.dll
            ${SFML_ROOT}/bin/sfml-system-d-3.dll
            ${SFML_ROOT}/bin/sfml-audio-d-3.dll
            ${SFML_ROOT}/bin/sfml-network-d-3.dll
        )
    else()
        # SFML 3.0.0 DLLs might be named without the "-3" suffix or with a version (e.g., 3.0)
        # Check both patterns
        file(GLOB GRAPHICS_DLL 
            "${SFML_ROOT}/bin/sfml-graphics-3.dll"
            "${SFML_ROOT}/bin/sfml-graphics-3.0.dll"
            "${SFML_ROOT}/bin/sfml-graphics.dll")
        file(GLOB WINDOW_DLL 
            "${SFML_ROOT}/bin/sfml-window-3.dll"
            "${SFML_ROOT}/bin/sfml-window-3.0.dll"
            "${SFML_ROOT}/bin/sfml-window.dll")
        file(GLOB SYSTEM_DLL 
            "${SFML_ROOT}/bin/sfml-system-3.dll"
            "${SFML_ROOT}/bin/sfml-system-3.0.dll"
            "${SFML_ROOT}/bin/sfml-system.dll")
        file(GLOB AUDIO_DLL 
            "${SFML_ROOT}/bin/sfml-audio-3.dll"
            "${SFML_ROOT}/bin/sfml-audio-3.0.dll"
            "${SFML_ROOT}/bin/sfml-audio.dll")
        file(GLOB NETWORK_DLL 
            "${SFML_ROOT}/bin/sfml-network-3.dll"
            "${SFML_ROOT}/bin/sfml-network-3.0.dll"
            "${SFML_ROOT}/bin/sfml-network.dll")
            
        # Message to debug what was found
        message(STATUS "Found GRAPHICS_DLL: ${GRAPHICS_DLL}")
        
        set(SFML_DLLS
            ${GRAPHICS_DLL}
            ${WINDOW_DLL}
            ${SYSTEM_DLL}
            ${AUDIO_DLL}
            ${NETWORK_DLL}
        )
    endif()
    
    message(STATUS "SFML DLLs to copy: ${SFML_DLLS}")
    
    # Copy DLLs to build directory (both for IDE and command line builds)
    foreach(DLL ${SFML_DLLS})
        if(EXISTS "${DLL}")
            add_custom_command(TARGET CheckersGame POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL}
                $<TARGET_FILE_DIR:CheckersGame>
            )
        else()
            message(WARNING "DLL not found: ${DLL}")
        endif()
    endforeach()
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
if(NOT APPLE AND NOT WIN32)
    target_include_directories(CheckersGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 