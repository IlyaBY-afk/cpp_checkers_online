#pragma once

#include <SFML/Graphics.hpp>

enum class PieceColor {
    White,
    Black
};

class Piece {
public:
    Piece(int row, int col, PieceColor color);
    void draw(sf::RenderWindow& window, float cellSize);
    void move(int row, int col);
    void promote();
    
    int getRow() const;
    int getCol() const;
    PieceColor getColor() const;
    bool isKing() const;
    bool isAlive() const;
    void setAlive(bool alive);
    static int getColFromX(float x, float cellSize);
    static int getRowFromY(float y, float cellSize, int boardSize);
    
private:
    int m_row;
    int m_col;
    PieceColor m_color;
    bool m_isKing;
    bool m_isAlive;
    sf::CircleShape m_shape;
}; 