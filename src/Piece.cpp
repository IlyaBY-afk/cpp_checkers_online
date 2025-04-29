#include "../include/Piece.hpp"

Piece::Piece(int row, int col, PieceColor color)
    : m_row(row), m_col(col), m_color(color), m_isKing(false), m_isAlive(true) {
    // Set piece color and initial appearance
    m_shape.setFillColor(color == PieceColor::White ? sf::Color::White : sf::Color::Black);
}

void Piece::draw(sf::RenderWindow& window, float cellSize) {
    if (!m_isAlive) return;
    
    // Position the piece
    float radius = cellSize * 0.4f;
    float x = m_col * cellSize + cellSize / 2;
    float y = m_row * cellSize + cellSize / 2;
    
    // Set up the piece appearance
    m_shape.setRadius(radius);
    m_shape.setOrigin(sf::Vector2f(radius, radius));
    m_shape.setPosition(sf::Vector2f(x, y));
    
    // Add outline
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOutlineColor(sf::Color::Black);
    
    // Draw the piece
    window.draw(m_shape);
    
    // Draw a crown for kings
    if (m_isKing) {
        sf::CircleShape crown(radius * 0.5f);
        crown.setOrigin(sf::Vector2f(radius * 0.5f, radius * 0.5f));
        crown.setPosition(sf::Vector2f(x, y));
        crown.setFillColor(sf::Color::Yellow);
        window.draw(crown);
    }
}

void Piece::move(int row, int col) {
    m_row = row;
    m_col = col;
}

void Piece::promote() {
    m_isKing = true;
}

int Piece::getRow() const {
    return m_row;
}

int Piece::getCol() const {
    return m_col;
}

PieceColor Piece::getColor() const {
    return m_color;
}

bool Piece::isKing() const {
    return m_isKing;
}

bool Piece::isAlive() const {
    return m_isAlive;
}

void Piece::setAlive(bool alive) {
    m_isAlive = alive;
} 