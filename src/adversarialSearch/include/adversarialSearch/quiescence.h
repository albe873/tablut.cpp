// quiescence.h

#ifndef QUIESCENCE_H
#define QUIESCENCE_H

#include "vgame.h"
#include <functional>

template <typename S, typename A, typename P, typename U>
class Quiescence {
private:
    const VGame<S, A, P, U>& game;
    int searchDepth;
    std::function<U(S, P)> eval;
    std::function<U(S, P, int)> evalTerminal;


public:

    Quiescence(const VGame<S, A, P, U>& game, int searchDepth, 
        std::function<U(const S&, const P&)> eval, 
        std::function<U(const S&, const P&, int)> evalTerminal)
    : game(game), searchDepth(searchDepth), eval(eval), evalTerminal(evalTerminal) 
    {}

    void setSearchDepth(int newDepth) {
        this->searchDepth = newDepth;
    }


    U qMax(const S& state, const P& player, U alpha, U beta, int depth) {
        if (game.isTerminal(state))
            return evalTerminal(state, player, searchDepth);
        
        if (depth == 0)
            return eval(state, player);

        auto value = eval(state, player);
        if (value >= beta)
            return value;
        if (alpha < value)
            alpha = value;
        
        auto best = value;

        auto actions = game.getActions(state);
        for (const auto& action : actions) {

            // Check if the action is quiet
            auto newState = game.getResult(state, action);
            if (!game.isTerminal(newState) && game.isQuiet(state, newState))
                continue;

            // examinate the capture
            value = qMin(newState, player, alpha, beta, depth - 1);
            if (value >= beta)
                return value;
            if (value > best)
                best = value;
            if (value > alpha)
                alpha = value;
        }
        return best;
    }

    U qMin(const S& state, const P& player, U alpha, U beta, int depth) {
        if (game.isTerminal(state))
            return evalTerminal(state, player, searchDepth);

        if (depth == 0)
            return eval(state, player);

        auto value = eval(state, player);
        if (value <= alpha)
            return value;
        if (beta > value)
            beta = value;
        
        auto best = value;


        auto actions = game.getActions(state);
        for (const auto& action : actions) {
            auto newState = game.getResult(state, action);
            
            if (!game.isTerminal(newState) && game.isQuiet(state, newState))
                continue;

            // examinate the capture
            value = qMax(newState, player, alpha, beta, depth - 1);

            if (value <= alpha)
                return value;
            if (value < best)
                best = value;
            if (value < beta)
                beta = value;
        }
        return best;
    }

};

#endif // QUIESCENCE_H