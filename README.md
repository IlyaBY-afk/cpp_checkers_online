# Checkers Game with SFML

A simple checkers (draughts) game implemented in C++ using SFML graphics library.

## Requirements

- C++ compiler with C++17 support
- SFML 2.5 or later
- CMake 3.10 or later

## Building the Game

```bash
# Create a build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make
```

## Running the Game

```bash
# From the build directory
./CheckersGame
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