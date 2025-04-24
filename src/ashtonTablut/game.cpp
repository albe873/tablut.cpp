// game.cpp

#include "tablut/game.h"

Game::Game(State initialState,
    std::function<std::vector<Move>(State)> actionsFunction,
    std::function<State(State, Move)> resultFunction,
    std::function<int(State, Turn)> utilityFunction,
    int util_min, int util_max, int util_unknown) :
    VGame<State, Move, Turn, int>(util_min, util_max, util_unknown)
{
    
    this->initialState = initialState;
    this->actionsFunction = actionsFunction;
    this->resultFunction = resultFunction;
    this->utilityFunction = utilityFunction;
}


State Game::getInitialState() const {
    return initialState;
}

std::vector<Turn> Game::getPlayers() const {
    return {Turn::Black, Turn::White};
}

Turn Game::getPlayer(const State& state) const {
    return state.getTurn();
}

std::vector<Move> Game::getActions(const State& state) const {
    return actionsFunction(state);
}
State Game::getResult(State state, const Move& action) const {
    return resultFunction(state, action);
}

bool Game::isTerminal(const State& state) const {
    return state.getTurn() == Turn::BlackWin || state.getTurn() == Turn::WhiteWin || state.getTurn() == Turn::Draw;
}

int Game::getUtility(const State& state, const Turn& player) const {
    return utilityFunction(state, player);
}

bool Game::isQuiet(const State& state, const State& newState) const {
    if (state.getBlackPieces() != newState.getBlackPieces() || 
        state.getWhitePieces() != newState.getWhitePieces())
        return false;

    if (newState.getTurn() == Turn::White && kingEscapeRoutes(newState) != 0)
        return false;

    return true;
}



inline int Game::kingEscapeRoutes(const State& state) const {
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