#ifndef BNS_H
#define BNS_H

#include <vector>
#include <limits>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <numeric>
#include <iostream>

#include "vgame.h"
#include "utilities.h"

/* 
Implementation of the BNS algorithm by (I don't remember what) LLM
*** does not work well ***
should perform similar or better than MTD(f) (at least I read that in some forums about chess programming)
but I don't have time to fix it
*/

template <typename S, typename A, typename P, typename U>
class bns {
private:
    void inline updateMetrics(int depth) {
        #ifdef ENABLE_METRICS
            metrics.updateMaxDepth(depth);
            metrics.incrementNodesExpanded();
        #endif
    }

protected:
    const VGame<S, A, P, U>& game;
    Timer timer;
    SimpleMetrics metrics;
    int startDepthLimit; // Renamed from searchDepthLimit
    int currentDepthLimit; // Added for ID
    bool hEvalUsed = false;

    // --- Core BNS Helper Functions ---

    U nextGuess(U alpha, U beta, int subtreeCount) {
        if (subtreeCount <= 1) {
            return alpha + (beta - alpha) / 2;
        }
        double factor = static_cast<double>(subtreeCount - 1) / subtreeCount;
        return static_cast<U>(alpha + std::floor((beta - alpha) * factor));
    }

    U alphabetaTest(S state, P player, U alpha, U beta, int depth) {
        updateMetrics(depth);

        if (depth == 0)
            return eval(state, player);
        
        if (game.isTerminal(state)) {
            return game.getUtility(state, player);
        }

        P currentPlayer = game.getPlayer(state);
        std::vector<A> actions = game.getActions(state);

        if (currentPlayer == player) {
            U value = game.util_min;
            for (const auto& action : actions) {
                if (timer.isTimeOut()) return alpha;
                S newState = game.getResult(state, action);
                value = std::max(value, alphabetaTest(newState, player, alpha, beta, depth - 1));
                if (value >= beta) {
                    return beta;
                }
                alpha = std::max(alpha, value);
            }
            return alpha;
        } else {
            U value = game.util_max;
            for (const auto& action : actions) {
                if (timer.isTimeOut()) return beta;
                S newState = game.getResult(state, action);
                value = std::min(value, alphabetaTest(newState, player, alpha, beta, depth - 1));
                if (value <= alpha) {
                    return alpha;
                }
                beta = std::min(beta, value);
            }
            return beta;
        }
    }

    virtual std::vector<A> orderActions(const S& /*state*/, std::vector<A> actions, const P& /*player*/, const int& /*depth*/) {
        return actions;
    }

    virtual U eval(const S& state, const P& player) {
        return game.getUtility(state, player);
    }

    virtual U evalTerminal(const S& state, const P& player, const int& distance) {
        return game.getUtility(state, player);
    }

    std::pair<A, U> runBNSLoop(S state, P player, const std::vector<A>& orderedActions) {
        U alpha = game.util_min;
        U beta = game.util_max;
        int subtreeCount = orderedActions.size();
        A bestAction = orderedActions.empty() ? A{} : orderedActions[0];
        A confirmedBestAction = bestAction;

        while (beta - alpha >= 2 && !timer.isTimeOut()) {
            U test = nextGuess(alpha, beta, subtreeCount);
            int betterCount = 0;
            A currentBestActionInLoop = orderedActions.empty() ? A{} : orderedActions[0];

            for (int i = 0; i < orderedActions.size(); ++i) {
                if (timer.isTimeOut()) break;
                const A& action = orderedActions[i];
                S newState = game.getResult(state, action);

                U value = alphabetaTest(newState, player, test - 1, test, currentDepthLimit - 1);

                if (timer.isTimeOut()) break;

                if (value >= test) {
                    betterCount++;
                    currentBestActionInLoop = action;
                }
            }

            if (timer.isTimeOut()) break;

            if (betterCount == 0) {
                beta = test - 1;
            } else {
                alpha = test;
                if (betterCount == 1) {
                    confirmedBestAction = currentBestActionInLoop;
                }
            }
        }

        return {confirmedBestAction, alpha};
    }

public:
    bns(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
        : game(game), timer(maxTimeSeconds), startDepthLimit(startDepth)
    {}

    std::pair<A, U> makeDecision(S state) {
        metrics.reset();
        timer.start();
        P player = game.getPlayer(state);
        std::vector<A> actions = game.getActions(state);

        if (actions.empty()) return {{}, game.getUtility(state, player)};
        if (actions.size() == 1) return {actions[0], game.util_unknown};

        A overallBestAction = actions[0];
        U overallBestUtility = game.util_min;

        currentDepthLimit = 0;

        std::vector<A> orderedActions = actions;

        do {
            currentDepthLimit++;

            std::pair<A, U> resultThisDepth = runBNSLoop(state, player, orderedActions);

            if (timer.isTimeOut()) {
                break;
            }

            overallBestAction = resultThisDepth.first;
            overallBestUtility = resultThisDepth.second;

            auto it = std::find(actions.begin(), actions.end(), overallBestAction);
            if (it != actions.end()) {
                std::iter_swap(actions.begin(), it);
                orderedActions = actions;
            } else {
                orderedActions = actions;
            }

        } while (!timer.isTimeOut());

        return {overallBestAction, overallBestUtility};
    }

    std::string getMetrics() const {
        #ifdef ENABLE_METRICS
            return metrics.toString();
        #else
            return "Metrics disabled.";
        #endif
    }
};

#endif // BNS_H