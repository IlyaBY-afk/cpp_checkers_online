#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Piece.hpp"

class Board {
public:
    struct MoveResult {
        bool moved = false;
        bool captured = false;
        bool canChain = false;
    };

    Board(float boardSize);
    ~Board();
    
    void draw(sf::RenderWindow& window);
    void initializePieces();
    bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol);
    Piece* getPieceAt(int row, int col);
    MoveResult handleClick(float x, float y, PieceColor currentPlayer);
    std::pair<int, int> getBoardPosition(float x, float y);
    bool playerHasAnyCapture(PieceColor color);
    
    // Getters for the last move
    int getLastMoveFromRow() const { return m_lastMoveFromRow; }
    int getLastMoveFromCol() const { return m_lastMoveFromCol; }
    int getLastMoveToRow() const { return m_lastMoveToRow; }
    int getLastMoveToCol() const { return m_lastMoveToCol; }

private:
    const int BOARD_SIZE = 8;
    float m_cellSize;
    float m_boardSize;
    sf::RectangleShape m_cells[8][8];
    std::vector<Piece*> m_pieces;
    
    Piece* m_selectedPiece = nullptr;
    int m_selectedRow = -1;
    int m_selectedCol = -1;
    
    // Track last move for network play
    int m_lastMoveFromRow = -1;
    int m_lastMoveFromCol = -1;
    int m_lastMoveToRow = -1;
    int m_lastMoveToCol = -1;
    
    bool canCapture(Piece* piece, int toRow, int toCol, int& capturedRow, int& capturedCol);
    void capturePiece(int row, int col);
    void checkForPromotion(Piece* piece);
    bool hasValidMoves(Piece* piece);
    bool hasValidCapture(Piece* piece);
}; 