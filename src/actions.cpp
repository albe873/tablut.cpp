// actions.cpp

#include "actions.h"

inline bool Action::isEmpty(cord c, State s) {
    return s.getPiece(c.x, c.y) == Piece::Empty;
}
inline bool Action::isThrone(cord c, State s) {
    return c == s.throne;
}
inline bool Action::isCamp(cord c, State s) {
    for (cord camp : s.camps) {
        if (c == camp)
            return true;
    }
    return false;
}
inline bool Action::checksIfValid(cord start, cord dest, Piece piece, State s) {
    // Check if the piece is NOT empty or the throne
    if (isThrone(dest, s) || !isEmpty(dest, s))
        return false;

    // Check if the new position is a camp (prohibited for white)
    if ((piece == Piece::White || piece == Piece::King) && isCamp(dest, s))
        return false;
    else {      // Check if the new position is a camp (prohibited return for black)
        if (isCamp(dest, s) && !isCamp(start, s))
            return false;
    }

    return true;
}


std::vector<Move> Action::getActions(State s) {
    std::vector<Move> moves;
    Turn turn = s.getTurn();

    // Check if the game is over
    if (turn != Turn::Black && turn != Turn::White) {
        return moves;
    }

    // Piece type to move, based on the turn
    Piece rightPiece = (turn == Turn::Black) ? Piece::Black : Piece::White;

    // coordinates, start and current (to be used in the loop)
    cord start, dest;

    // Main loop to check all pieces on the board
    for (int8_t y = 0; y < s.size; y++) {
        for (int8_t x = 0; x < s.size; x++) {

            start = cord(x, y);
            
            // if is empty, skip
            if (isEmpty(start, s)) {
                continue;
            }

            // if is not the right piece, skip
            if (s.getPiece(x, y) == rightPiece || (turn == Turn::White && s.getPiece(x, y) == Piece::King)) {
                int8_t newX, newY;

                // Check all 4 directions
                // Up
                newY = y - 1;
                newX = x;
                while (newY >= 0) {
                    dest = cord(newX, newY);

                    if (!checksIfValid(start, dest, rightPiece, s))
                        break;
                    
                    moves.push_back(Move(start, dest));
                    y--;
                }

                // Down
                newY = y + 1;
                while (newY < s.size) {
                    dest = cord(newX, newY);

                    if (!checksIfValid(start, dest, rightPiece, s))
                        break;

                    moves.push_back(Move(start, dest));
                    newY++;
                }

                // Left
                newX = x - 1;
                newY = y;
                while (newX >= 0) {
                    dest = cord(newX, newY);

                    if (!checksIfValid(start, dest, rightPiece, s))
                        break;

                    moves.push_back(Move(start, dest));
                    newX--;
                }

                // Right
                newX = x + 1;
                while (newX < s.size) {
                    dest = cord(newX, newY);

                    if (!checksIfValid(start, dest, rightPiece, s))
                        break;

                    moves.push_back(Move(start, dest));
                    newX++;
                }
            }
        }
    }

    return moves;
}


