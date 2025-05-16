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
   - Make sure the extracted folder has this structure:
     ```
     C:\SFML\
        ├── bin\          # Contains .dll files
        ├── include\      # Contains header files
        ├── lib\          # Contains .lib files and cmake directory
        │    └── cmake\SFML\
        └── ...
     ```

2. **Configure with CMake**:
   ```
   cmake -S. -Bbuild -DSFML_DIR=C:/SFML/lib/cmake/SFML
   ```
   Use forward slashes (/) in the path, even on Windows.

3. **Build the project**:
   ```
   cmake --build build --config Release
   ```

4. **Run the game**:
   - The necessary SFML DLLs will be automatically copied to your build directory
   - Run `build\Release\CheckersGame.exe`

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