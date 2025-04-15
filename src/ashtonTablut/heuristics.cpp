// heuristics.cpp

#include "tablut/heuristics.h"

const int8_t Heuristics::max = 100;
const int8_t Heuristics::min = -100;

int8_t Heuristics::getHeuristics(const State& state, const Turn& turn) {
    if (state.getTurn() == Turn::BlackWin) {
        if (turn == Turn::Black) return Heuristics::max;
        else return Heuristics::min;
    }
    if (state.getTurn() == Turn::WhiteWin) {
        if (turn == Turn::White) return Heuristics::max;
        else return Heuristics::min;  
    }

    if (turn == Turn::White) {
        return whiteHeuristics(state);
    } else {
        return blackHeuristics(state);
    }
}

int8_t Heuristics::whiteHeuristics(const State& state) {
    if (state.getTurn() == Turn::Draw)
        return 10;

    int8_t score = 0;

    // escape routes
    switch (kingEscapeRoutes(state)) {
        case 1:
            score += 2;
            break;
        case 2:
            score += 4;
    }
    
    // pieces
    score += state.getWhitePieces() * 5;
    score -= state.getBlackPieces() * 5;

    // king surrounding
    score -= kingSurrounding(state)*2;

    // king position and escape routes
    if (state.getPiece(State::throne) == Piece::King)
        score += 2;

    return score;
}

int8_t Heuristics::blackHeuristics(const State& state) {
    if (state.getTurn() == Turn::Draw)
        return -10;
    int8_t score = 0;

    // escape routes
    switch (kingEscapeRoutes(state)) {
        case 1:
            score -= 3;
            break;
        case 2:
            score -= 6;
            break;
    }
    // king surrounding
    score += kingSurrounding(state)*2;

    // pieces
    score += state.getBlackPieces() * 5;
    score -= state.getWhitePieces() * 5;

    return score;
}

int8_t Heuristics::kingSurrounding(const State& state) {
    int8_t black = 0;
    cord kingPos = state.getKingPosition();
    for (cord dir : Directions::ALL_DIRECTIONS) {
        cord dest = Move::calculateNewCord(kingPos, dir);
        if (state.getPiece(dest) == Piece::Black)
            black++;
    }
    return black;
}


int8_t Heuristics::kingEscapeRoutes(const State& state) {
    int8_t escapes = 0;
    cord kingPos = state.getKingPosition();
    if (kingPos.x >= 3 && kingPos.x <= 5 && kingPos.y >= 3 && kingPos.y <= 5) {
        return 0; // King is near the throne
    }

    int8_t x = kingPos.x;
    int8_t y = kingPos.y;

    // Check all 4 directions
    // not using the cord directions for speed (if it even matters)
    // Up
    int8_t newY = y - 1;
    int8_t newX = x;
    cord dest;
    while (newY >= 0) {
        dest = cord(newX, newY);
        if (!state.isEmpty(dest))
            break;
        newY--;
    }
    if (newY == -1)
        escapes++;

    // Down
    newY = y + 1;
    while (newY < state.size) {
        dest = cord(newX, newY);
        if (!state.isEmpty(dest))
            break;
        newY++;
    }
    if (newY == state.size) {
        escapes++;
        if (escapes == 2)
            return escapes;
    }

    // Left
    newX = x - 1;
    newY = y;
    while (newX >= 0) {
        dest = cord(newX, newY);
        if (!state.isEmpty(dest))
            break;
        newX--;
    }
    if (newX == -1) {
        escapes++;
        if (escapes == 2)
            return escapes;
    }

    // Right
    newX = x + 1;
    while (newX < state.size) {
        dest = cord(newX, newY);
        if (!state.isEmpty(dest))
            break;
        newX++;
    }
    if (newX == state.size) {
        escapes++;
        if (escapes == 2)
            return escapes;
    }

    return escapes;
}