// result.cpp

#include "result.h"
#include "actions.h"

inline void checkCaptureWhite(State s, cord c, cord dir) {
    cord captureCord = Move::calculateNewCord(c, dir);

    // piece
    // enemy
    // piece or throne or camp
    // -> capture

    if (s.getPiece(captureCord) == Piece::Black) {
        cord checkCord = Move::calculateNewCord(captureCord, dir);
        
        Piece checkPiece = s.getPiece(checkCord);

        // check if the piece is empty or cord invalid (0 and -1)
        if ( (int8_t) checkPiece < 1)
            return;

        
        if (checkPiece == Piece::White || s.isThrone(checkCord) || s.isCamp(checkCord)) {
            // capture the piece
            s.removePiece(captureCord);
        }
    }
}

inline bool checkCaptureBlack(State s, cord c, cord dir) {
    cord captureCord = Move::calculateNewCord(c, dir);

    // black
    // white
    // black or throne or camp
    // -> capture

    // if toCapture is king, use the function to check if the king is captured
    if (s.getPiece(captureCord) == Piece::King) {
        return checkKingCapture(s, captureCord, dir);
    }

    // else normal capture check
    if (s.getPiece(captureCord) == Piece::White) {
        cord checkCord = Move::calculateNewCord(captureCord, dir);
        
        Piece checkPiece = s.getPiece(checkCord);

        // check if the piece is empty or cord invalid (0 and -1)
        if ( (int8_t) checkPiece < 1)
            return;

        
        if (checkPiece == Piece::Black || s.isThrone(checkCord) || s.isCamp(checkCord)) {
            // capture the piece
            s.removePiece(captureCord);
        }
    }

    return false;
}




inline bool checkKingCapture(State s, cord c, cord dir) {
    // if the king is on the throne, it needs to be surrounded by 4 black soldiers
    if (State::isThrone(c)) {
        for (cord dir : Directions::ALL_DIRECTIONS) {
            cord checkCord = Move::calculateNewCord(c, dir);
            if (s.getPiece(checkCord) != Piece::Black) {
                return false;
            }
        }
        return true;
    }

    // check if the king is near the throne
    bool isNearThrone = false;
    for (cord dir : Directions::ALL_DIRECTIONS) {
        cord isThrone = Move::calculateNewCord(c, dir);
        if (s.isThrone(isThrone)) {
            isNearThrone = true;
            break;
        }
    }

    // special case: needs to be surrounded by 3 black soldiers
    if (isNearThrone) {
        for (cord dir : Directions::ALL_DIRECTIONS) {
            cord checkCord = Move::calculateNewCord(c, dir);
            if (s.getPiece(checkCord) != Piece::Black && !s.isThrone(checkCord)) {
                return false;
            }
        }
        return true;
    }

    // Normal case: 2 black soldiers are enaugh
    cord checkCord = Move::calculateNewCord(c, dir);
    Piece checkPiece = s.getPiece(checkCord);

    // check if the piece is empty or cord invalid (0 and -1)
    if ( (int8_t) checkPiece < 1)
        return false;
    
    // check if the piece is black or camp (NOT throne, is special case above)
    if (checkPiece == Piece::Black || s.isCamp(checkCord))
        return true;

    return false;
}

State Result::applyAction(State s, Move m) {
    
    // clone the state
    State newState = s.clone();

    // get the piece to move
    Piece toMove = newState.getPiece(m.getFrom());
    
    // move the piece
    newState.movePiece(m.getFrom(), m.getTo());

    // check if the piece is a king && if is on an escape tile
    if (toMove == Piece::King && (m.getTo().x == 0 || m.getTo().x == 8 || m.getTo().y == 0 || m.getTo().y == 8)) {
        // if the king is on the edges, it wins
        // note: already checked if m.getTo() is a valid position, so only escapes tiles are possible
        newState.setTurn(Turn::WhiteWin);
        return newState;
    }

    // captures checks
    if (toMove == Piece::Black) {
        for (cord dir : Directions::ALL_DIRECTIONS) {
            // return true if the king is captured
            if (checkCaptureBlack(newState, m.getTo(), dir)) {
                newState.setTurn(Turn::BlackWin);
                return newState;
            }
        }

        // if the piece is black, change the turn to white
        newState.setTurn(Turn::White);

        // check if the enemy cannot move
        if (!Action::isPossibleToMove(newState))
            newState.setTurn(Turn::WhiteWin);
    } else {
        for (cord dir : Directions::ALL_DIRECTIONS) {
            checkCaptureWhite(newState, m.getTo(), dir);
        }

        // if the piece is white, change the turn to black
        newState.setTurn(Turn::Black);

        // check if the enemy cannot move
        if (!Action::isPossibleToMove(newState))
            newState.setTurn(Turn::BlackWin);
    }

    // check if the game is a draw
    if (newState.isHistoryRepeated())
        newState.setTurn(Turn::Draw);
    
    return newState;
}