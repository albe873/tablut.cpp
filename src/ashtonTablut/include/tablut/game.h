// game.h

#include "adversarialSearch/vgame.h"
#include "state.h"
#include <functional>


class Game : public VGame<State, Move, Turn, int8_t> {
private:
    State initialState;
    std::function<std::vector<Move>(State)> actionsFunction;
    std::function<State(State, Move)> resultFunction;
    std::function<int8_t(State, Turn)> utilityFunction;

public:
    // Constructor
    Game(State initialState,
         std::function<std::vector<Move>(State)> actionsFunction,
         std::function<State(State, Move)> resultFunction,
         std::function<int8_t(State, Turn)> utilityFunction);
    
    
    State getInitialState() const override;

    std::vector<Turn> getPlayers() const override;

    Turn getPlayer(const State&) const override;

    std::vector<Move> getActions(const State&) const override;

    State getResult(State, const Move&) const override;

    bool isTerminal(const State&) const override;

    int8_t getUtility(const State&, const Turn&) const override;
};
