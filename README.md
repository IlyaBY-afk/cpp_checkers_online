# Checkers Game with SFML

A simple checkers (draughts) game implemented in C++ using SFML graphics library.

## Requirements

- C++ compiler with C++17 support
- SFML 3.0.0
- CMake 3.10 or later

## Building on Windows

1. **Install SFML**:
   - Download SFML 3.0.0 from the [official website](https://www.sfml-dev.org/download.php)
   - Get the right version for your compiler (MinGW or MSVC)
   - Extract to a known location (e.g., `C:\SFML`)
   - The folder structure should look like:
     ```
     C:\SFML\
        ├── bin\          # Contains .dll files
        ├── include\      # Contains header files
        ├── lib\          # Contains .lib files and cmake directory
        └── ...
     ```
   - **IMPORTANT**: Check the `lib` folder and confirm it contains files like:
     - sfml-graphics.lib
     - sfml-window.lib
     - sfml-system.lib
   - If the library files have different names (like `sfml-graphics-3.lib` or with version numbers), you'll need to either rename them or modify CMakeLists.txt

2. **Configure with CMake**:
   ```
   # Clean build directory if it exists
   rmdir /s /q build
   mkdir build
   
   # Configure with correct SFML path
   cmake -S. -Bbuild -DSFML_DIR=C:/SFML/lib/cmake/SFML
   ```
   Use forward slashes (/) in paths, even on Windows. Replace `C:/SFML` with your actual SFML path.

3. **Troubleshooting CMake**:
   If you encounter linking errors, check the CMake output for diagnostics. It will tell you:
   - The path it's looking for SFML in
   - Whether specific files were found
   - Available library files
   
   Common issues include:
   - Wrong path to SFML
   - Library files with unexpected names (like version suffixes)
   - Missing SFML components

4. **Build the project**:
   ```
   cmake --build build --config Release
   ```

5. **Run the game**:
   - Navigate to the build directory and find the executable:
     ```
     cd build\Release
     CheckersGame.exe
     ```
   - If you get any errors about missing DLLs, copy all .dll files from `C:\SFML\bin` to your executable's directory

## Building on macOS

1. **Install SFML**:
   ```
   brew install sfml
   ```

2. **Configure and build**:
   ```
   rm -rf build  # Clear any previous build
   mkdir build
   cmake -S. -Bbuild
   cmake --build build
   ```

3. **Run the game**:
   ```
   ./build/CheckersGame
   ```

## Building on Linux

1. **Install SFML**:
   ```
   sudo apt-get install libsfml-dev
   ```

2. **Configure and build**:
   ```
   cmake -S. -Bbuild
   cmake --build build
   ```

3. **Run the game**:
   ```
   ./build/CheckersGame
   ```

## Controls

- **Left Mouse Button**: Select and move pieces
- **R Key**: Reset the game
- **Escape Key**: Exit the game

## Game Rules

- Red pieces move first
- Regular pieces can only move diagonally forward
- Kings can move diagonally in any direction
- Capture opponent pieces by jumping over them
- Multiple captures in one turn are allowed
- Pieces are promoted to kings when they reach the opposite end of the board

## Project Structure

- `src/`: Source files
- `include/`: Header files
- `assets/`: Game assets (images, sounds, etc.)
- `build/`: Build directory (generated) 