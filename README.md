# Checkers Game with SFML

A simple checkers (draughts) game implemented in C++ using SFML graphics library.

## Requirements

- C++ compiler with C++17 support
- SFML 2.5 or later
- CMake 3.10 or later

## Building on Windows

1. **Install SFML**:
   - Download SFML from the [official website](https://www.sfml-dev.org/download.php)
   - Get the right version for your compiler (MinGW or MSVC)
   - Extract to a known location (e.g., `C:\SFML`)

2. **Configure with CMake**:
   ```
   cmake -S. -Bbuild -DSFML_DIR=C:/SFML/lib/cmake/SFML
   ```
   Replace `C:/SFML` with your actual SFML installation path. The SFML_DIR should point to the directory containing SFMLConfig.cmake.

3. **Build the project**:
   ```
   cmake --build build --config Release
   ```

4. **Run the game**:
   - Make sure all necessary SFML DLLs are in your build directory or in your system PATH
   - Run `build\Release\CheckersGame.exe`

## Building on macOS/Linux

1. **Install SFML**:
   - macOS: `brew install sfml`
   - Ubuntu/Debian: `sudo apt-get install libsfml-dev`

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