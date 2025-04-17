// game.cpp

#include "tablut/game.h"

Game::Game(State initialState,
    std::function<std::vector<Move>(State)> actionsFunction,
    std::function<State(State, Move)> resultFunction,
    std::function<int(State, Turn)> utilityFunction)
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