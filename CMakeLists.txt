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
    # On Windows, link to the appropriate libraries
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(CheckersGame 
            ${SFML_LIB_DIR}/sfml-graphics-d.lib
            ${SFML_LIB_DIR}/sfml-window-d.lib
            ${SFML_LIB_DIR}/sfml-system-d.lib
            ${SFML_LIB_DIR}/sfml-audio-d.lib
            ${SFML_LIB_DIR}/sfml-network-d.lib
        )
    else()
        target_link_libraries(CheckersGame 
            ${SFML_LIB_DIR}/sfml-graphics.lib
            ${SFML_LIB_DIR}/sfml-window.lib
            ${SFML_LIB_DIR}/sfml-system.lib
            ${SFML_LIB_DIR}/sfml-audio.lib
            ${SFML_LIB_DIR}/sfml-network.lib
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
        set(SFML_DLLS
            ${SFML_ROOT}/bin/sfml-graphics-3.dll
            ${SFML_ROOT}/bin/sfml-window-3.dll
            ${SFML_ROOT}/bin/sfml-system-3.dll
            ${SFML_ROOT}/bin/sfml-audio-3.dll
            ${SFML_ROOT}/bin/sfml-network-3.dll
        )
    endif()
    
    # Copy DLLs to build directory (both for IDE and command line builds)
    foreach(DLL ${SFML_DLLS})
        add_custom_command(TARGET CheckersGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL}
            $<TARGET_FILE_DIR:CheckersGame>
        )
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