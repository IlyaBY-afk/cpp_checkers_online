#include "../include/Board.hpp"
#include <iostream>

Board::Board(float boardSize)
    : m_boardSize(boardSize) {
    m_cellSize = boardSize / BOARD_SIZE;
    
    // Create the checkered board pattern
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            m_cells[row][col].setSize(sf::Vector2f(m_cellSize, m_cellSize));
            m_cells[row][col].setPosition(sf::Vector2f(col * m_cellSize, row * m_cellSize));
            
            // Alternating colors for the checkerboard pattern
            if ((row + col) % 2 == 0) {
                m_cells[row][col].setFillColor(sf::Color(240, 217, 181)); // Light squares
            } else {
                m_cells[row][col].setFillColor(sf::Color(181, 136, 99));  // Dark squares
            }
        }
    }
    
    initializePieces();
}

Board::~Board() {
    // Clean up piece memory
    for (auto piece : m_pieces) {
        delete piece;
    }
    m_pieces.clear();
}

void Board::draw(sf::RenderWindow& window) {
    // Draw the board
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            window.draw(m_cells[row][col]);
            
            // Highlight the selected cell
            if (row == m_selectedRow && col == m_selectedCol) {
                sf::RectangleShape highlight(sf::Vector2f(m_cellSize, m_cellSize));
                highlight.setPosition(sf::Vector2f(col * m_cellSize, row * m_cellSize));
                highlight.setFillColor(sf::Color(255, 255, 0, 100)); // Semi-transparent yellow
                window.draw(highlight);
            }
        }
    }
    
    // Draw the pieces
    for (auto piece : m_pieces) {
        if (piece->isAlive()) {
            piece->draw(window, m_cellSize);
        }
    }
}

void Board::initializePieces() {
    // Clear existing pieces if any
    for (auto piece : m_pieces) {
        delete piece;
    }
    m_pieces.clear();
    
    // Create pieces for both players
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            // Pieces only go on "dark" squares
            if ((row + col) % 2 == 1) {
                // Black pieces on top rows
                if (row < 3) {
                    m_pieces.push_back(new Piece(row, col, PieceColor::Black));
                }
                // White pieces on bottom rows
                else if (row > 4) {
                    m_pieces.push_back(new Piece(row, col, PieceColor::White));
                }
            }
        }
    }
}

Piece* Board::getPieceAt(int row, int col) {
    for (auto piece : m_pieces) {
        if (piece->isAlive() && piece->getRow() == row && piece->getCol() == col) {
            return piece;
        }
    }
    return nullptr;
}

std::pair<int, int> Board::getBoardPosition(float x, float y) {
    int col = static_cast<int>(x / m_cellSize);
    int row = static_cast<int>(y / m_cellSize);
    
    // Ensure coordinates are within bounds
    if (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
        return {row, col};
    }
    
    // Return an invalid position if out of bounds
    return {-1, -1};
}

Board::MoveResult Board::handleClick(float x, float y, PieceColor currentPlayer) {
    MoveResult result;
    auto [row, col] = getBoardPosition(x, y);
    
    // Click out of bounds
    if (row == -1 || col == -1) return result;
    
    bool mustCapture = playerHasAnyCapture(currentPlayer);
    // If a piece is already selected
    if (m_selectedPiece) {
        // Only allow moving if the selected piece belongs to the current player
        if (m_selectedPiece->getColor() != currentPlayer) {
            // Deselect if not the current player's piece
            m_selectedPiece = nullptr;
            m_selectedRow = -1;
            m_selectedCol = -1;
            return result;
        }
        // Try to move the piece
        int fromRow = m_selectedRow, fromCol = m_selectedCol;
        int absRowDiff = std::abs(row - fromRow);
        int absColDiff = std::abs(col - fromCol);
        bool isCaptureAttempt = (absRowDiff > 1 && absRowDiff == absColDiff);
        if (mustCapture && !isCaptureAttempt) {
            // Must capture, but this is not a capture move
            return result;
        }
        if (movePiece(fromRow, fromCol, row, col)) {
            result.moved = true;
            // Check if this was a capture
            if (absRowDiff > 1) {
                result.captured = true;
                // After a capture, check if the same piece can capture again
                Piece* movedPiece = getPieceAt(row, col);
                if (movedPiece && hasValidCapture(movedPiece)) {
                    // Keep the piece selected for chaining
                    m_selectedPiece = movedPiece;
                    m_selectedRow = row;
                    m_selectedCol = col;
                    result.canChain = true;
                    return result;
                }
            }
            // Move successful, clear selection
            m_selectedPiece = nullptr;
            m_selectedRow = -1;
            m_selectedCol = -1;
            return result;
        }
        // Clicked on the same piece, deselect it
        else if (row == m_selectedRow && col == m_selectedCol) {
            m_selectedPiece = nullptr;
            m_selectedRow = -1;
            m_selectedCol = -1;
            return result;
        }
        // Clicked on a different piece, select that one (if it belongs to the current player)
        else if (Piece* newPiece = getPieceAt(row, col)) {
            if (newPiece->getColor() == currentPlayer) {
                m_selectedPiece = newPiece;
                m_selectedRow = row;
                m_selectedCol = col;
            }
            return result;
        }
    }
    // No piece selected yet, try to select one (only if it belongs to the current player)
    else {
        Piece* piece = getPieceAt(row, col);
        if (piece && piece->getColor() == currentPlayer) {
            // If must capture, only allow selecting pieces that can capture
            if (mustCapture && !hasValidCapture(piece)) {
                return result;
            }
            m_selectedPiece = piece;
            m_selectedRow = row;
            m_selectedCol = col;
        }
    }
    return result;
}

bool Board::isValidMove(int fromRow, int fromCol, int toRow, int toCol) {
    // Check if destination is within bounds
    if (toRow < 0 || toRow >= BOARD_SIZE || toCol < 0 || toCol >= BOARD_SIZE) {
        return false;
    }
    // Check if destination is occupied
    if (getPieceAt(toRow, toCol)) {
        return false;
    }
    // Check if destination is a "dark" square
    if ((toRow + toCol) % 2 == 0) {
        return false;
    }
    Piece* piece = getPieceAt(fromRow, fromCol);
    if (!piece) {
        return false;
    }
    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    int absRowDiff = std::abs(rowDiff);
    int absColDiff = std::abs(colDiff);
    // Pieces can only move diagonally
    if (absRowDiff != absColDiff) {
        return false;
    }
    if (!piece->isKing()) {
        // Regular pieces: only move forward for non-captures
        if (absRowDiff == 1) {
            if ((piece->getColor() == PieceColor::White && rowDiff != -1) ||
                (piece->getColor() == PieceColor::Black && rowDiff != 1)) {
                return false;
            }
            return true;
        }
        // Allow captures in any diagonal direction
        if (absRowDiff == 2) {
            int capturedRow, capturedCol;
            return canCapture(piece, toRow, toCol, capturedRow, capturedCol);
        }
        return false;
    } else {
        // King: can move any number of squares diagonally if path is clear or if making a valid distant capture
        int stepRow = (rowDiff > 0) ? 1 : -1;
        int stepCol = (colDiff > 0) ? 1 : -1;
        int r = fromRow + stepRow, c = fromCol + stepCol;
        bool foundOpponent = false;
        while (r != toRow && c != toCol) {
            Piece* p = getPieceAt(r, c);
            if (p) {
                if (p->getColor() == piece->getColor()) {
                    return false; // Blocked by own piece
                }
                if (foundOpponent) {
                    return false; // More than one opponent in the path
                }
                foundOpponent = true;
            }
            r += stepRow;
            c += stepCol;
        }
        if (foundOpponent) {
            // This is a capture move
            return true;
        } else {
            // No pieces in the path: normal king move
            // Disallow if any forced capture exists for the player
            if (playerHasAnyCapture(piece->getColor())) {
                return false;
            }
            return true;
        }
    }
}

bool Board::canCapture(Piece* piece, int toRow, int toCol, int& capturedRow, int& capturedCol) {
    int fromRow = piece->getRow();
    int fromCol = piece->getCol();
    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    int absRowDiff = std::abs(rowDiff);
    int absColDiff = std::abs(colDiff);
    if (!piece->isKing()) {
        // Regular piece: must move exactly 2 squares diagonally
        if (absRowDiff != 2 || absColDiff != 2) return false;
        capturedRow = fromRow + rowDiff / 2;
        capturedCol = fromCol + colDiff / 2;
        Piece* capturedPiece = getPieceAt(capturedRow, capturedCol);
        if (!capturedPiece || capturedPiece->getColor() == piece->getColor()) {
            return false;
        }
        return true;
    } else {
        // King: can capture any opponent piece along the diagonal, landing on any empty square after
        if (absRowDiff != absColDiff) return false;
        int stepRow = (rowDiff > 0) ? 1 : -1;
        int stepCol = (colDiff > 0) ? 1 : -1;
        int r = fromRow + stepRow, c = fromCol + stepCol;
        bool foundOpponent = false;
        capturedRow = -1;
        capturedCol = -1;
        while (r != toRow && c != toCol) {
            Piece* p = getPieceAt(r, c);
            if (p) {
                if (p->getColor() == piece->getColor()) {
                    return false; // Blocked by own piece
                }
                if (foundOpponent) {
                    return false; // More than one opponent in the path
                }
                foundOpponent = true;
                capturedRow = r;
                capturedCol = c;
            }
            r += stepRow;
            c += stepCol;
        }
        // Must have found exactly one opponent piece, and at least one empty cell after it
        if (!foundOpponent) return false;
        // Check that all cells after the captured piece up to toRow/toCol are empty
        int afterRow = capturedRow + stepRow;
        int afterCol = capturedCol + stepCol;
        while (afterRow != toRow || afterCol != toCol) {
            if (getPieceAt(afterRow, afterCol)) return false;
            afterRow += stepRow;
            afterCol += stepCol;
        }
        return true;
    }
}

void Board::capturePiece(int row, int col) {
    Piece* piece = getPieceAt(row, col);
    if (piece) {
        piece->setAlive(false);
    }
}

bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    // Check if the move is valid
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    Piece* piece = getPieceAt(fromRow, fromCol);
    if (!piece) {
        return false;
    }

    // Check if this is a capture move
    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    bool isTwoStep = (std::abs(rowDiff) > 1);

    if (isTwoStep) {
        // For a capture, need to ensure there's an opponent's piece to capture
        int capturedRow, capturedCol;
        if (canCapture(piece, toRow, toCol, capturedRow, capturedCol)) {
            // Perform the capture and move
            capturePiece(capturedRow, capturedCol);
            piece->move(toRow, toCol);
            
            // Store the last move
            m_lastMoveFromRow = fromRow;
            m_lastMoveFromCol = fromCol;
            m_lastMoveToRow = toRow;
            m_lastMoveToCol = toCol;
            
            // Check for promotion
            checkForPromotion(piece);
            return true;
        }
        return false;
    } else {
        // Regular move (non-capture)
        piece->move(toRow, toCol);
        
        // Store the last move
        m_lastMoveFromRow = fromRow;
        m_lastMoveFromCol = fromCol;
        m_lastMoveToRow = toRow;
        m_lastMoveToCol = toCol;
        
        // Check for promotion
        checkForPromotion(piece);
        return true;
    }
}

void Board::checkForPromotion(Piece* piece) {
    if (!piece || piece->isKing()) {
        return;
    }
    
    // Red pieces are promoted at the bottom row
    if (piece->getColor() == PieceColor::White && piece->getRow() == BOARD_SIZE - 1) {
        piece->promote();
    }
    // Black pieces are promoted at the top row
    else if (piece->getColor() == PieceColor::Black && piece->getRow() == 0) {
        piece->promote();
    }
}

bool Board::hasValidMoves(Piece* piece) {
    if (!piece || !piece->isAlive()) {
        return false;
    }
    
    int row = piece->getRow();
    int col = piece->getCol();
    
    // Check all possible moves for this piece
    int directions[4][2] = {
        {-1, -1}, {-1, 1},  // Up-left, Up-right
        {1, -1}, {1, 1}     // Down-left, Down-right
    };
    
    // For non-king pieces, limit the directions they can move
    int startDir = 0;
    int endDir = 4;
    
    if (!piece->isKing()) {
        if (piece->getColor() == PieceColor::White) {
            // White non-king pieces can only move up
            startDir = 0;
            endDir = 2;
        } else {
            // Black non-king pieces can only move down
            startDir = 2;
            endDir = 4;
        }
    }
    
    // Check all valid directions
    for (int i = startDir; i < endDir; i++) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];
        
        // Check for regular move
        if (isValidMove(row, col, newRow, newCol)) {
            return true;
        }
        
        // Check for jump move
        newRow = row + 2 * directions[i][0];
        newCol = col + 2 * directions[i][1];
        if (isValidMove(row, col, newRow, newCol)) {
            return true;
        }
    }
    
    return false;
}

// Helper to check if a piece has a valid capture
bool Board::hasValidCapture(Piece* piece) {
    if (!piece || !piece->isAlive()) {
        return false;
    }
    int row = piece->getRow();
    int col = piece->getCol();
    if (!piece->isKing()) {
        // Regular piece: check all 4 directions for adjacent captures
        int directions[4][2] = {
            {-1, -1}, {-1, 1},  // Up-left, Up-right
            {1, -1}, {1, 1}   // Down-left, Down-right
        };
        for (int i = 0; i < 4; i++) {
            int newRow = row + 2 * directions[i][0];
            int newCol = col + 2 * directions[i][1];
            int capturedRow, capturedCol;
            if (isValidMove(row, col, newRow, newCol) && canCapture(piece, newRow, newCol, capturedRow, capturedCol)) {
                return true;
            }
        }
        return false;
    } else {
        // King: check all 4 diagonals for any possible distant capture
        int directions[4][2] = {
            {-1, -1}, {-1, 1},  // Up-left, Up-right
            {1, -1}, {1, 1}   // Down-left, Down-right
        };
        for (int i = 0; i < 4; i++) {
            int stepRow = directions[i][0];
            int stepCol = directions[i][1];
            int r = row + stepRow;
            int c = col + stepCol;
            bool foundOpponent = false;
            while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                Piece* p = getPieceAt(r, c);
                if (p) {
                    if (p->getColor() == piece->getColor() || foundOpponent) {
                        break; // Blocked by own piece or more than one opponent
                    }
                    foundOpponent = true;
                } else if (foundOpponent) {
                    // Empty square after opponent: valid capture landing
                    int capturedRow = r - stepRow;
                    int capturedCol = c - stepCol;
                    // Use canCapture to double-check
                    if (canCapture(piece, r, c, capturedRow, capturedCol)) {
                        return true;
                    }
                }
                r += stepRow;
                c += stepCol;
            }
        }
        return false;
    }
}

bool Board::playerHasAnyCapture(PieceColor color) {
    for (auto piece : m_pieces) {
        if (piece->isAlive() && piece->getColor() == color) {
            if (hasValidCapture(piece)) {
                return true;
            }
        }
    }
    return false;
}

int Piece::getColFromX(float x, float cellSize) {
    return static_cast<int>(x / cellSize);
}

int Piece::getRowFromY(float y, float cellSize, int boardSize) {
    return boardSize - 1 - static_cast<int>(y / cellSize);
} 