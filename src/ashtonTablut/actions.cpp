// actions.cpp

#include "tablut/actions.h"

// Only check for current position, not the entire line
inline bool Action::checksIfValid(const cord& start, const cord& dest, Piece piece, const State& s) {
    // Check if the piece is NOT empty or the throne
    if (State::isThrone(dest) || !s.isEmpty(dest))
        return false;

    // Check if the new position is a camp
    if (State::isCamp(dest)) {

        // camp logic for black
        if (piece == Piece::Black) {
            // cannot return to a camp
            if (!State::isCamp(start))
                return false;
            // cannot cross the table to another camp
            int deltaX = dest.x - start.x;
            int deltaY = dest.y - start.y;
            if (deltaX > 2 || deltaX < -2 || deltaY > 2 || deltaY < -2)
                return false;
        
        // camp logic for white
        } else { // piece == Piece::White or Piece::King
            // white cannot go to camps
            return false;
        }
    }

    return true;
}


std::vector<Move> Action::getActions(const State& s) {
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
    for (int y = 0; y < s.size; y++) {
        for (int x = 0; x < s.size; x++) {

            start = cord(x, y);
            
            // if is empty, skip
            if (s.isEmpty(start)) {
                continue;
            }

            // if is not the right piece, skip
            if (s.getPiece(start) == rightPiece || (turn == Turn::White && s.getPiece(start) == Piece::King)) {
                int8_t newX, newY;

                // Check all 4 directions
                // not using the cord directions for speed (if it even matters)
                // Up
                newY = y - 1;
                newX = x;
                while (newY >= 0) {
                    dest = cord(newX, newY);

                    if (!checksIfValid(start, dest, rightPiece, s))
                        break;
                    
                    moves.push_back(Move(start, dest));
                    newY--;
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


bool Action::isPossibleToMove(const State& s) {
    Turn turn = s.getTurn();
    Piece rightPiece = (turn == Turn::Black) ? Piece::Black : Piece::White;

    cord start, dest;

        // Main loop to check all pieces on the board
    for (int y = 0; y < s.size; y++) {
        for (int x = 0; x < s.size; x++) {

            start = cord(x, y);
            
            // if is empty, skip
            if (s.isEmpty(start)) {
                continue;
            }

            // if is not the right piece, skip
            if (s.getPiece(start) == rightPiece || (turn == Turn::White && s.getPiece(start) == Piece::King)) {
                int newX, newY;

                // Check all 4 directions
                for (cord dir : Directions::ALL_DIRECTIONS) {
                    dest = Move::calculateNewCord(start, dir);

                    // Check if the destination is valid
                    if (checksIfValid(start, dest, rightPiece, s))
                        return true;
                }
            }
        }
    }

    // No valid moves found
    return false;
}


