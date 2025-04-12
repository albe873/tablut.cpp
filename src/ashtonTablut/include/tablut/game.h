// game.h

#include "adversarialSearch/vgame.h"
#include "state.h"
#include <functional>


class Game : public VGame<State, Move, Turn, int8_t> {
private:
    State initialState;
    std::function<std::vector<Move>(State)> actionsFunction;
    std::function<State(State, Move)> resultFunction;
    std::function<int8_t(State)> utilityFunction;

public:
    // Constructor
    Game(State initialState,
         std::function<std::vector<Move>(State)> actionsFunction,
         std::function<State(State, Move)> resultFunction,
         std::function<int8_t(State)> utilityFunction);
    
    
    State getInitialState() override;

    std::vector<Turn> getPlayers() override;

    Turn getPlayer(State state) override;

    std::vector<Move> getActions(State state) override;

    State getResult(State state, Move action) override;

    bool isTerminal(State state) override;

    int8_t getUtility(State state) override;
};
