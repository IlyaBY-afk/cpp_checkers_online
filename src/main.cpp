#include <iostream>
#include "../include/Game.hpp"

int main() {
    try {
        // Create a game with a 800x800 window
        Game game(800, 800);
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
} 