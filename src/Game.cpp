#include "../include/Game.hpp"
#include <iostream>

struct MoveResult {
    bool moved = false;
    bool captured = false;
    bool canChain = false;
};

Game::Game(int windowWidth, int windowHeight)
    : m_window(sf::VideoMode({static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight)}), "Checkers Game"),
      m_board(nullptr),
      m_currentPlayer(PieceColor::White),
      m_gameOver(false),
      m_state(GameState::MainMenu),
      m_statusText(m_font),
      m_ipInputText(m_font),
      m_gameMode(GameMode::LocalGame),
      m_isMyTurn(true) {
    
    m_window.setFramerateLimit(60);
    
    // Calculate board size to fit the window
    float boardSize = std::min(windowWidth, windowHeight) * 0.9f;
    
    // Create the game board
    m_board = new Board(boardSize);
    
    // Load the font
    if (!m_font.openFromFile("fonts/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
    }
    
    // Initialize text objects after font is loaded
    m_statusText.setFont(m_font);
    m_statusText.setString("");
    m_statusText.setCharacterSize(16);
    m_statusText.setFillColor(sf::Color::White);
    m_statusText.setPosition({10.f, 10.f});
    
    m_ipInputText.setFont(m_font);
    m_ipInputText.setString("");
    m_ipInputText.setCharacterSize(20);
    m_ipInputText.setFillColor(sf::Color::White);
}

Game::~Game() {
    delete m_board;
}

void Game::run() {
    while (m_window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    // In SFML 3.0, events are returned as std::optional
    while (auto eventOpt = m_window.pollEvent()) {
        const sf::Event& event = *eventOpt; // Dereference to get the actual event
        
        // Handle events based on the current game state
        if (event.is<sf::Event::Closed>()) {
            m_window.close();
        } else if (m_state == GameState::MainMenu) {
            handleMainMenuEvents(event);
        } else if (m_state == GameState::MultiplayerMenu) {
            handleMultiplayerMenuEvents(event);
        } else if (m_state == GameState::HostMenu) {
            handleHostMenuEvents(event);
        } else if (m_state == GameState::JoinMenu) {
            handleJoinMenuEvents(event);
        } else if (m_state == GameState::Playing) {
            handleGamePlayEvents(event);
        }
    }
}

void Game::handleMainMenuEvents(const sf::Event& event) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            float w = m_window.getSize().x;
            float h = m_window.getSize().y;
            sf::FloatRect singlePlayerBtn(sf::Vector2f(w/2-150, h/2-100), sf::Vector2f(300, 50));
            sf::FloatRect multiplayerBtn(sf::Vector2f(w/2-150, h/2-20), sf::Vector2f(300, 50));
            sf::FloatRect exitBtn(sf::Vector2f(w/2-150, h/2+60), sf::Vector2f(300, 50));
            sf::Vector2f mousePos(mousePressed->position.x, mousePressed->position.y);
            
            if (singlePlayerBtn.contains(mousePos)) {
                startLocalGame();
            } else if (multiplayerBtn.contains(mousePos)) {
                m_state = GameState::MultiplayerMenu;
            } else if (exitBtn.contains(mousePos)) {
                m_window.close();
            }
        }
    }
}

void Game::handleMultiplayerMenuEvents(const sf::Event& event) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            float w = m_window.getSize().x;
            float h = m_window.getSize().y;
            sf::FloatRect hostBtn(sf::Vector2f(w/2-150, h/2-100), sf::Vector2f(300, 50));
            sf::FloatRect joinBtn(sf::Vector2f(w/2-150, h/2-20), sf::Vector2f(300, 50));
            sf::FloatRect backBtn(sf::Vector2f(w/2-150, h/2+60), sf::Vector2f(300, 50));
            sf::Vector2f mousePos(mousePressed->position.x, mousePressed->position.y);
            
            if (hostBtn.contains(mousePos)) {
                m_state = GameState::HostMenu;
                m_network.hostGame();
            } else if (joinBtn.contains(mousePos)) {
                m_state = GameState::JoinMenu;
                m_ipAddress = "";
                m_ipInputText.setString(m_ipAddress);
            } else if (backBtn.contains(mousePos)) {
                m_state = GameState::MainMenu;
            }
        }
    }
}

void Game::handleHostMenuEvents(const sf::Event& event) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            float w = m_window.getSize().x;
            float h = m_window.getSize().y;
            sf::FloatRect backBtn(sf::Vector2f(w/2-150, h/2+100), sf::Vector2f(300, 50));
            sf::Vector2f mousePos(mousePressed->position.x, mousePressed->position.y);
            
            if (backBtn.contains(mousePos)) {
                m_network.stopHosting();
                m_state = GameState::MultiplayerMenu;
            }
        }
    }
    
    // Check if connection established
    if (m_network.getStatus() == NetworkStatus::Connected) {
        startNetworkGame(GameMode::NetworkHost);
    }
}

void Game::handleJoinMenuEvents(const sf::Event& event) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            float w = m_window.getSize().x;
            float h = m_window.getSize().y;
            sf::FloatRect connectBtn(sf::Vector2f(w/2-150, h/2+20), sf::Vector2f(300, 50));
            sf::FloatRect backBtn(sf::Vector2f(w/2-150, h/2+100), sf::Vector2f(300, 50));
            sf::Vector2f mousePos(mousePressed->position.x, mousePressed->position.y);
            
            if (connectBtn.contains(mousePos)) {
                if (!m_ipAddress.empty()) {
                    m_network.connectToGame(m_ipAddress);
                }
            } else if (backBtn.contains(mousePos)) {
                m_network.disconnect();
                m_state = GameState::MultiplayerMenu;
            }
        }
    } else if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
        // Handle text input for IP address
        if (textEntered->unicode == 8 && !m_ipAddress.empty()) { // Backspace
            m_ipAddress.pop_back();
        } else if ((textEntered->unicode >= '0' && textEntered->unicode <= '9') || 
                   textEntered->unicode == '.') {
            m_ipAddress += static_cast<char>(textEntered->unicode);
        }
        m_ipInputText.setString(m_ipAddress);
    }
    
    // Check if connection established
    if (m_network.getStatus() == NetworkStatus::Connected) {
        startNetworkGame(GameMode::NetworkClient);
    }
}

void Game::handleGamePlayEvents(const sf::Event& event) {
    if (m_gameOver) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::R) {
                if (m_gameMode == GameMode::LocalGame) {
                    startLocalGame();
                } else {
                    // Return to main menu for network games
                    m_network.disconnect();
                    m_state = GameState::MainMenu;
                }
            } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_network.disconnect();
                m_state = GameState::MainMenu;
            }
        }
        return;
    }
    
    // During gameplay - handle mouse clicks
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            // In network games, only allow moves on your turn
            if (m_gameMode != GameMode::LocalGame && !m_isMyTurn) {
                return;
            }
            
            Board::MoveResult moveResult = m_board->handleClick(mousePressed->position.x, mousePressed->position.y, m_currentPlayer);
            if (moveResult.moved) {
                // In network game, send the move
                if (m_gameMode != GameMode::LocalGame) {
                    int fromRow = m_board->getLastMoveFromRow();
                    int fromCol = m_board->getLastMoveFromCol();
                    int toRow = m_board->getLastMoveToRow();
                    int toCol = m_board->getLastMoveToCol();
                    m_network.sendMove(fromRow, fromCol, toRow, toCol);
                    m_isMyTurn = false;
                }
                
                // If no chain capture is possible, switch player
                if (!moveResult.canChain) {
                    switchPlayer();
                }
                
                // Check if game is over
                if (isGameOver()) {
                    m_gameOver = true;
                    if (m_currentPlayer == PieceColor::White) {
                        m_winnerText = "Black Wins!";
                    } else {
                        m_winnerText = "White Wins!";
                    }
                }
            }
        }
    } else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            // Return to main menu
            m_network.disconnect();
            m_state = GameState::MainMenu;
        }
    }
}

void Game::update() {
    // For network games, check for received moves
    if (m_gameMode != GameMode::LocalGame && !m_isMyTurn && m_network.hasReceivedMove()) {
        NetworkMove move = m_network.getReceivedMove();
        if (m_board->movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
            m_isMyTurn = true;
            switchPlayer();
            
            // Check if game is over
            if (isGameOver()) {
                m_gameOver = true;
                if (m_currentPlayer == PieceColor::White) {
                    m_winnerText = "Black Wins!";
                } else {
                    m_winnerText = "White Wins!";
                }
            }
        }
    }
}

void Game::render() {
    m_window.clear(sf::Color(50, 50, 50));
    
    // Render based on the current game state
    if (m_state == GameState::MainMenu) {
        renderMainMenu();
    } else if (m_state == GameState::MultiplayerMenu) {
        renderMultiplayerMenu();
    } else if (m_state == GameState::HostMenu) {
        renderHostMenu();
    } else if (m_state == GameState::JoinMenu) {
        renderJoinMenu();
    } else if (m_state == GameState::Playing) {
        // Draw the board in playing state
        m_board->draw(m_window);
        
        // Draw game over text if game is over
        if (m_gameOver) {
            sf::RectangleShape overlay(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            m_window.draw(overlay);
            
            sf::Text gameOverText(m_font);
            gameOverText.setString(m_winnerText);
            gameOverText.setCharacterSize(40);
            gameOverText.setFillColor(sf::Color::White);
            gameOverText.setPosition({
                m_window.getSize().x / 2 - gameOverText.getLocalBounds().size.x / 2,
                static_cast<float>(m_window.getSize().y) / 2 - 80
            });
            m_window.draw(gameOverText);
            
            sf::Text restartText(m_font);
            restartText.setString("Press R to Restart or ESC for Menu");
            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition({
                m_window.getSize().x / 2 - restartText.getLocalBounds().size.x / 2,
                static_cast<float>(m_window.getSize().y) / 2
            });
            m_window.draw(restartText);
        } else {
            // Draw player turn text
            sf::Text playerText(m_font);
            playerText.setString(m_currentPlayer == PieceColor::White ? "White's Turn" : "Black's Turn");
            playerText.setCharacterSize(20);
            playerText.setFillColor(sf::Color::White);
            playerText.setPosition({20, 20});
            m_window.draw(playerText);
            
            // Draw network status if in network game
            if (m_gameMode != GameMode::LocalGame) {
                sf::Text networkText(m_font);
                networkText.setString(m_isMyTurn ? "Your Turn" : "Opponent's Turn");
                networkText.setCharacterSize(20);
                networkText.setFillColor(m_isMyTurn ? sf::Color::Green : sf::Color::Red);
                networkText.setPosition({20, static_cast<float>(m_window.getSize().y - 40)});
                m_window.draw(networkText);
            }
        }
    }
    
    m_window.display();
}

void Game::switchPlayer() {
    m_currentPlayer = (m_currentPlayer == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

bool Game::isGameOver() {
    // Check if the current player has any valid moves
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = m_board->getPieceAt(row, col);
            if (piece && piece->isAlive() && piece->getColor() == m_currentPlayer) {
                // Check for any valid moves for this piece
                for (int newRow = 0; newRow < 8; newRow++) {
                    for (int newCol = 0; newCol < 8; newCol++) {
                        if (m_board->isValidMove(row, col, newRow, newCol)) {
                            return false;  // Found a valid move, game not over
                        }
                    }
                }
            }
        }
    }
    return true;  // No valid moves, game over
}

sf::RectangleShape Game::createButton(float x, float y, float width, float height, const sf::Color& color) {
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setPosition({x, y});
    button.setFillColor(color);
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(2);
    return button;
}

sf::Text Game::createButtonText(const std::string& string, float x, float y, unsigned int size) {
    sf::Text text(m_font);
    text.setString(string);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition({
        x - text.getLocalBounds().size.x/2, 
        y - text.getLocalBounds().size.y/2
    });
    return text;
}

void Game::renderMainMenu() {
    float w = m_window.getSize().x;
    float h = m_window.getSize().y;
    
    sf::Text title(m_font);
    title.setString("Checkers Game");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::White);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition({
        w/2 - bounds.size.x/2, 
        h/4 - bounds.size.y/2
    });
    m_window.draw(title);
    
    auto singlePlayerBtn = createButton(w/2-150, h/2-100, 300, 50, sf::Color(100, 100, 200));
    auto multiplayerBtn = createButton(w/2-150, h/2-20, 300, 50, sf::Color(100, 100, 200));
    auto exitBtn = createButton(w/2-150, h/2+60, 300, 50, sf::Color(200, 100, 100));
    
    m_window.draw(singlePlayerBtn);
    m_window.draw(multiplayerBtn);
    m_window.draw(exitBtn);
    
    auto singlePlayerText = createButtonText("Single Player", w/2, h/2-75, 24);
    auto multiplayerText = createButtonText("Multiplayer", w/2, h/2+5, 24);
    auto exitText = createButtonText("Exit", w/2, h/2+85, 24);
    
    m_window.draw(singlePlayerText);
    m_window.draw(multiplayerText);
    m_window.draw(exitText);
}

void Game::renderMultiplayerMenu() {
    float w = m_window.getSize().x;
    float h = m_window.getSize().y;
    
    sf::Text title(m_font);
    title.setString("Multiplayer");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::White);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition({
        w/2 - bounds.size.x/2, 
        h/4 - bounds.size.y/2
    });
    m_window.draw(title);
    
    auto hostBtn = createButton(w/2-150, h/2-100, 300, 50, sf::Color(100, 100, 200));
    auto joinBtn = createButton(w/2-150, h/2-20, 300, 50, sf::Color(100, 100, 200));
    auto backBtn = createButton(w/2-150, h/2+60, 300, 50, sf::Color(200, 100, 100));
    
    m_window.draw(hostBtn);
    m_window.draw(joinBtn);
    m_window.draw(backBtn);
    
    auto hostText = createButtonText("Host Game", w/2, h/2-75, 24);
    auto joinText = createButtonText("Join Game", w/2, h/2+5, 24);
    auto backText = createButtonText("Back", w/2, h/2+75, 24);
    
    m_window.draw(hostText);
    m_window.draw(joinText);
    m_window.draw(backText);
}

void Game::renderHostMenu() {
    float w = m_window.getSize().x;
    float h = m_window.getSize().y;
    
    sf::Text title(m_font);
    title.setString("Host Game");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::White);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition({
        w/2 - bounds.size.x/2, 
        h/4 - bounds.size.y/2
    });
    m_window.draw(title);
    
    sf::Text ipText(m_font);
    ipText.setString("Your IP: " + m_network.getLocalIpAddress());
    ipText.setCharacterSize(24);
    ipText.setFillColor(sf::Color::White);
    bounds = ipText.getLocalBounds();
    ipText.setPosition({
        w/2 - bounds.size.x/2, 
        h/2-50
    });
    m_window.draw(ipText);
    
    sf::Text statusText(m_font);
    statusText.setString(m_network.getStatusText());
    statusText.setCharacterSize(20);
    statusText.setFillColor(sf::Color::White);
    bounds = statusText.getLocalBounds();
    statusText.setPosition({
        w/2 - bounds.size.x/2, 
        h/2
    });
    m_window.draw(statusText);
    
    auto backBtn = createButton(w/2-150, h/2+100, 300, 50, sf::Color(200, 100, 100));
    m_window.draw(backBtn);
    
    auto backText = createButtonText("Cancel", w/2, h/2+115, 24);
    m_window.draw(backText);
}

void Game::renderJoinMenu() {
    float w = m_window.getSize().x;
    float h = m_window.getSize().y;
    
    sf::Text title(m_font);
    title.setString("Join Game");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::White);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition({
        w/2 - bounds.size.x/2, 
        h/4 - bounds.size.y/2
    });
    m_window.draw(title);
    
    sf::Text ipPrompt(m_font);
    ipPrompt.setString("Enter Host IP:");
    ipPrompt.setCharacterSize(24);
    ipPrompt.setFillColor(sf::Color::White);
    bounds = ipPrompt.getLocalBounds();
    ipPrompt.setPosition({
        w/2 - bounds.size.x/2, 
        h/2-100
    });
    m_window.draw(ipPrompt);
    
    auto ipBox = createButton(w/2-150, h/2-50, 300, 40, sf::Color(80, 80, 80));
    m_window.draw(ipBox);
    
    bounds = m_ipInputText.getLocalBounds();
    m_ipInputText.setPosition({
        w/2 - bounds.size.x/2, 
        h/2-45
    });
    m_window.draw(m_ipInputText);
    
    auto connectBtn = createButton(w/2-150, h/2+20, 300, 50, sf::Color(100, 200, 100));
    auto backBtn = createButton(w/2-150, h/2+100, 300, 50, sf::Color(200, 100, 100));
    
    m_window.draw(connectBtn);
    m_window.draw(backBtn);
    
    auto connectText = createButtonText("Connect", w/2, h/2+35, 24);
    auto backText = createButtonText("Back", w/2, h/2+115, 24);
    
    m_window.draw(connectText);
    m_window.draw(backText);
    
    sf::Text statusText(m_font);
    statusText.setString(m_network.getStatusText());
    statusText.setCharacterSize(20);
    statusText.setFillColor(sf::Color::White);
    bounds = statusText.getLocalBounds();
    statusText.setPosition({
        w/2 - bounds.size.x/2, 
        static_cast<float>(h)/2+170
    });
    m_window.draw(statusText);
}

void Game::startLocalGame() {
    // Reset the board
    delete m_board;
    float boardSize = std::min(m_window.getSize().x, m_window.getSize().y) * 0.9f;
    m_board = new Board(boardSize);
    
    // Set up game state
    m_gameMode = GameMode::LocalGame;
    m_currentPlayer = PieceColor::White;
    m_gameOver = false;
    m_state = GameState::Playing;
}

void Game::startNetworkGame(GameMode mode) {
    // Reset the board
    delete m_board;
    float boardSize = std::min(m_window.getSize().x, m_window.getSize().y) * 0.9f;
    m_board = new Board(boardSize);
    
    // Set up game state
    m_gameMode = mode;
    m_currentPlayer = PieceColor::White;
    m_gameOver = false;
    
    // Set player turn based on role
    m_isMyTurn = (mode == GameMode::NetworkHost);
    
    m_state = GameState::Playing;
} 