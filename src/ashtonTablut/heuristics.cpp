// heuristics.cpp

#include "tablut/heuristics.h"

const int Heuristics::max = 1000;
const int Heuristics::min = -1000;

const int Heuristics::whiteDraw = 100;
const int Heuristics::blackDraw = -100;


// weight constants for white
const int w_bp = 55;        // prize
const int w_wp = 50;        // penalty
const int w_best_pos = 2;   // prize
const int w_k_esc  = 10;    // prize
const int w_k_surr = 10;    // penalty
const int w_k_surr_nt = 5;  // penalty

// wheight constants for black
const int b_bp = 45;        // penalty
const int b_wp = 50;        // prize 
const int b_best_pos = 2;   // prize
const int b_k_esc  = 10;    // penalty
const int b_k_surr = 10;    // prize
const int b_k_surr_nt = 5;  // prize


int Heuristics::getHeuristics(const State& state, const Turn& player) {
    Turn turn = state.getTurn();

    // win logic
    if (turn == Turn::BlackWin)
        return (player == Turn::Black) ? max : min;
    
    if (turn == Turn::WhiteWin)
        return (player == Turn::White) ? max : min;
    
    // draw logic
    if (turn == Turn::Draw)
        return drawLogic(state, player);
    
    // game evaluetion logic
    if (player == Turn::White)
        return whiteHeuristics(state);
    else
        return blackHeuristics(state);
}

int Heuristics::whiteHeuristics(const State& state) {
    int score = 0;

    // escape routes
    score += kingEscapeRoutes(state) * w_k_esc;
    
    // pieces
    int8_t whitePieces = state.getWhitePieces();
    score += whitePieces * w_wp;
    score -= state.getBlackPieces() * w_bp;

    // best positions
    if (6 - whitePieces <= 0) // only if there are more than 5 pieces (6, 7, 8)
        score += whiteInBestPositions(state) * w_best_pos;

    // king surrounding
    if (state.getKingPosition() == State::throne || kingNearThrone(state))
        score -= kingSurrounding(state) * w_k_surr_nt;
    else
        score -= kingSurrounding(state) * w_k_surr;

    // king position and escape routes
    if (state.getPiece(State::throne) == Piece::King)
        score += w_k_esc;

    return score;
}

inline int Heuristics::blackHeuristics(const State& state) {
    int score = 0;

    // escape routes
    score -= kingEscapeRoutes(state) * b_k_esc;

    // king surrounding
    if (state.getKingPosition() == State::throne || kingNearThrone(state))
        score += kingSurrounding(state) * b_k_surr_nt;
    else
        score += kingSurrounding(state) * b_k_surr;


    // pieces
    score += state.getBlackPieces() * b_bp;
    score -= state.getWhitePieces() * b_wp;

    return score;
}


inline int Heuristics::drawLogic(const State& state, const Turn& player) {
    // white player: if I have more pieces, I should win, so a draw is a loss
    // if I have less pieces, a draw is a compromise that I should take
    if (player == Turn::White) { // white player
        if (state.getWhitePieces() > state.getBlackPieces())
            return min;
        else
            return whiteHeuristics(state) + whiteDraw;
    }

    // black player: same as above
    else {
        if (state.getWhitePieces() < state.getBlackPieces())
            return min;
        else
            return blackHeuristics(state) + blackDraw;  // black draw is negative
    }
}


inline int Heuristics::kingSurrounding(const State& state) {
    int black = 0;
    cord kingPos = state.getKingPosition();
    for (cord dir : Directions::ALL_DIRECTIONS) {
        cord dest = Move::calculateNewCord(kingPos, dir);
        if (state.getPiece(dest) == Piece::Black)
            black++;
    }
    return black;
}


inline int Heuristics::kingEscapeRoutes(const State& state) {
    int escapes = 0;
    cord kingPos = state.getKingPosition();
    if (kingPos.x >= 3 && kingPos.x <= 5 && kingPos.y >= 3 && kingPos.y <= 5) {
        return 0; // King is near the throne
    }

    int x = kingPos.x;
    int y = kingPos.y;

    // Check all 4 directions
    // not using the cord directions for speed (if it even matters)
    // Up
    int newY = y - 1;
    int newX = x;
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

const bool Heuristics::nearThroneMask[State::size][State::size] = {
    {false, false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false, false},
    {false, false, false, false, true , false, false, false, false},
    {false, false, false, true , false, true , false, false, false},
    {false, false, false, false, true, false, false, false, false},
    {false, false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false, false}
};

inline bool Heuristics::kingNearThrone(const State& state) {
    cord kingPos = state.getKingPosition();
    return nearThroneMask[kingPos.x][kingPos.y];
}

const std::vector<cord> Heuristics::bestPositionsWhite = {{2, 3}, {3, 5}, {5, 3}, {6, 5} };

inline int Heuristics::whiteInBestPositions(const State& state) {
    int score = 0;
    for (cord pos : bestPositionsWhite) {
        if (state.getPiece(pos) == Piece::White)
            score++;
    }
    return score;
}