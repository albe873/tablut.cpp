// game.cpp

#include "tablut/game.h"

Game::Game(State initialState,
    std::function<std::vector<Move>(State)> actionsFunction,
    std::function<State(State, Move)> resultFunction,
    std::function<int8_t(State)> utilityFunction)
{
    this->initialState = initialState;
    this->actionsFunction = actionsFunction;
    this->resultFunction = resultFunction;
    this->utilityFunction = utilityFunction;
}


State Game::getInitialState() {
    return initialState;
}

std::vector<Turn> Game::getPlayers() {
    return {Turn::Black, Turn::White};
}

Turn Game::getPlayer(State state) {
    return state.getTurn();
}

std::vector<Move> Game::getActions(State state) {
    return actionsFunction(state);
}
State Game::getResult(State state, Move action) {
    return resultFunction(state, action);
}

bool Game::isTerminal(State state) {
    return state.getTurn() == Turn::BlackWin || state.getTurn() == Turn::WhiteWin || state.getTurn() == Turn::Draw;
}

int8_t Game::getUtility(State state) {
    return utilityFunction(state);
}