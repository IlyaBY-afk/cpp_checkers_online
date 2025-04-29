#pragma once

#include <SFML/Graphics.hpp>
#include "Board.hpp"
#include "NetworkManager.hpp"
#include <string>

enum class GameState { MainMenu, Playing, GameOver, MultiplayerMenu, HostMenu, JoinMenu };
enum class GameMode { LocalGame, NetworkHost, NetworkClient };

class Game {
public:
    Game(int windowWidth, int windowHeight);
    ~Game();
    
    void run();
    
private:
    sf::RenderWindow m_window;
    Board* m_board;
    PieceColor m_currentPlayer;
    bool m_gameOver = false;
    std::string m_winnerText;
    GameState m_state = GameState::MainMenu;
    sf::Font m_font;
    
    // Network components
    NetworkManager m_network;
    std::string m_ipAddress;
    GameMode m_gameMode = GameMode::LocalGame;
    bool m_isMyTurn = true;
    
    // UI components
    sf::Text m_statusText;
    sf::Text m_ipInputText;
    
    void handleEvents();
    void handleMainMenuEvents(const sf::Event& event);
    void handleMultiplayerMenuEvents(const sf::Event& event);
    void handleHostMenuEvents(const sf::Event& event);
    void handleJoinMenuEvents(const sf::Event& event);
    void handleGamePlayEvents(const sf::Event& event);
    
    void update();
    void render();
    void renderMainMenu();
    void renderMultiplayerMenu();
    void renderHostMenu();
    void renderJoinMenu();
    void switchPlayer();
    bool isGameOver();
    
    // UI helper methods
    sf::RectangleShape createButton(float x, float y, float width, float height, const sf::Color& color);
    sf::Text createButtonText(const std::string& string, float x, float y, unsigned int size);
    
    // Game management
    void startLocalGame();
    void startNetworkGame(GameMode mode);
}; 