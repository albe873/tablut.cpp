// mtd.h

#include <set>
#include <omp.h>
#include <algorithm>
#include <vector>
#include <limits> // Required for numeric_limits

#include "vgame.h"
#include "utilities.h"
#include "t_table.h"
#include "quiescence.h" // Include if quiescence search is used

#ifndef MTD_H
#define MTD_H

using namespace std;

// Metrics are disabled by default
// they can be enabled by defining ENABLE_METRICS in this file: #define ENABLE_METRICS
// or (preferred way) by passing -DENABLE_METRICS to the compiler

template <typename S, typename A, typename P, typename U>
class mtd {
private:
    void inline updateMiss() {
        #ifdef ENABLE_METRICS
            metrics.incrementTTMiss();
        #endif
    }
    void inline updateHit() {
        #ifdef ENABLE_METRICS
            metrics.incrementTTHit();
        #endif
    }
    void inline updateMetrics(int depth) {
        #ifdef ENABLE_METRICS
            metrics.updateMaxDepth(depth);
            metrics.incrementNodesExpanded();
        #endif
    }

    // Alpha-beta with memory search function
    U alphaBeta(S& state, P& player, U alpha, U beta, int depth, bool maximizingPlayer) {
        updateMetrics(currentDepthLimit - depth);

        if (game.isTerminal(state))
            return evalTerminal(state, player, currentDepthLimit - depth);

        if (timer.isTimeOut())
            return maximizingPlayer ? game.util_min : game.util_max; // Return worst score on timeout

        // Check transposition table
        int best_action_index = 0; // Hint for move ordering
        auto hash = state.hash();
        entry_type flag = entry_type::exact; // Will be updated by probe
        auto value = table.probe(hash, alpha, beta, depth, best_action_index);
        if (value != game.util_unknown) {
            updateHit();
            return value;
        }
        updateMiss();

        if (depth == 0) {
            // Quiescence search
            // if (maximizingPlayer)
            //     return quiescence.qMax(game, state, player, alpha, beta, 2);
            // else
            //     return quiescence.qMin(game, state, player, alpha, beta, 2);
            
            // normal evaluation function
            return eval(state, player);
        }

        auto actions = orderActions(state, game.getActions(state), player, depth, best_action_index);
        int current_best_action_index = 0;

        if (maximizingPlayer) {
            value = game.util_min;
            U current_alpha = alpha;
            for (int i = 0; i < actions.size(); i++) {
                auto action = actions[i];
                auto newState = game.getResult(state, action);
                U childValue = alphaBeta(newState, player, current_alpha, beta, depth - 1, !maximizingPlayer);

                if (childValue > value) {
                    value = childValue;
                    current_best_action_index = i;
                }

                if (value >= beta) { // Beta cutoff
                    flag = entry_type::l_bound;
                    break;
                }

                current_alpha = max(current_alpha, value);
            }
            if (value > alpha && value < beta) // Check if it's an exact value within the original window
                flag = entry_type::exact;
            else if (value <= alpha) // Value is <= original alpha (Fail Low equivalent for Max node)
                flag = entry_type::u_bound; // Store as upper bound (unlikely)
        
        
        } else { // Minimizing player
            value = game.util_max;
            U current_beta = beta;

            for (int i = 0; i < actions.size(); i++) {
                auto action = actions[i];
                auto newState = game.getResult(state, action);
                U childValue = alphaBeta(newState, player, alpha, current_beta, depth - 1, !maximizingPlayer);

                if (childValue < value) {
                    value = childValue;
                    current_best_action_index = i;
                }

                if (value <= alpha) { // Alpha cutoff 
                    flag = entry_type::u_bound;
                    break;
                }

                current_beta = min(current_beta, value);
            }
            if (value > alpha && value < beta) // Check if it's an exact value within the original window
                flag = entry_type::exact;
            else if (value >= beta) // Value is >= original beta (Fail High equivalent for Min node)
                flag = entry_type::l_bound; // Store as lower bound (unlikely)
        }

        table.insert(hash, flag, value, depth, current_best_action_index);

        return value;
    }


    U mtdfSearch(S& state, P& player, U guess, int depth) {
        U g = guess;
        U upperBound = game.util_max;
        U lowerBound = game.util_min;

        while (lowerBound < upperBound) {
            if (timer.isTimeOut()) break;

            // Adjust beta for zero-window search. Add 1 if g is the lower bound to avoid infinite loops with discrete utilities.
            U beta = g;
            if (g == lowerBound)
                beta = g + 1;

            // Perform zero-window search (alpha = beta - 1)
            g = alphaBeta(state, player, beta - 1, beta, depth, false);

            // Update bounds based on the result
            if (g < beta)
                upperBound = g;
            else
                lowerBound = g;
        }
        return g; // The converged value is the minimax value
    }


protected:
    const VGame<S, A, P, U>& game;
    bool hEvalUsed;
    int currentDepthLimit;
    int startDepthLimit;
    Timer timer;
    SimpleMetrics metrics;
    t_table<U, A> table;
    Quiescence<S, A, P, U> quiescence;

    // --- Virtual functions (can be overridden by derived classes) ---

    virtual void incrementDepthLimit() {
        this->currentDepthLimit++;
    }

    virtual bool isSignificantlyBetter(const U& newUtility, const U& bestUtility) {
        return false;
    }

    virtual bool hasSafeWinner(const U& resultUtility) {
        return resultUtility <= game.util_min || resultUtility >= game.util_max;
    }

    virtual U eval(const S& state, const P& player) {
        hEvalUsed = true;
        return game.getUtility(state, player);
    }

    virtual U evalTerminal(const S& state, const P& player, const int& distance) {
        return game.getUtility(state, player);
    }

    virtual vector<A> orderActions(const S& state, vector<A> actions,
                                   const P& player, const int& depth, const int& best_action_hint) {
        if (best_action_hint > 0 && best_action_hint < actions.size())
            std::swap(actions[0], actions[best_action_hint]);
        return actions;
    }


public:

    // Constructor
    mtd(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
    : game(game), startDepthLimit(startDepth), timer(maxTimeSeconds), table(game.util_unknown),
      quiescence(game, startDepth,
                 [this](const S& s, const P& p){ return this->eval(s, p); },
                 [this](const S& s, const P& p, const int& d){ return this->evalTerminal(s, p, d); }) // Setup quiescence if used
    {}

    pair<A, U> makeDecision(S state) {
        metrics.reset();
        table.clear();
        timer.start();

        currentDepthLimit = startDepthLimit;

        auto player = game.getPlayer(state);

        // get actions and put them in results array 
        auto actions = orderActions(state, game.getActions(state), player, currentDepthLimit, 0);
        vector<actionUtility<A, U>> results;
        for (auto action : actions)
            results.push_back({action, game.util_min});

        U best_util = game.util_min;
        U first_guess = eval(state, player); // Initial guess for MTD(f)
        vector<actionUtility<A, U>> new_results;;

        // Iterative Deepening Loop
        do {
            incrementDepthLimit();
            quiescence.setSearchDepth(currentDepthLimit);
            hEvalUsed = false;
            new_results.resize(results.size());

            int maxI = 0;
            #pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < results.size(); i++) {

                auto& actUtil = results[i];
                auto new_state = game.getResult(state, actUtil.action);

                // Use the utility from the previous depth for this specific action as a guess,
                // falling back to the overall best guess if it's the first action or unavailable.
                U guess;
                if (currentDepthLimit > startDepthLimit + 1)
                    guess = actUtil.utility;
                else
                    guess = first_guess;

                actUtil.utility = mtdfSearch(new_state, player, guess, currentDepthLimit - 1);

                // If the search is not timed out, update the maximum index for the results vector
                if (!timer.isTimeOut()) {
                    #pragma omp critical
                    {
                        maxI = max(i, maxI);
                    }
                }

                // Update the new results vector with the action and its utility
                new_results[i] = actUtil;
            }
            new_results.resize(maxI + 1);

            // Sort the results and update only if the timer is not timed out
            // to discard the last partial results 
            if (!timer.isTimeOut()) {
                std::sort(new_results.begin(), new_results.end());
                results = new_results;
            }

            if (!results.empty()) {
                // save best utility
                best_util = results[0].utility;
                // use the best utility for the next guess
                first_guess = best_util;
                
                if (hasSafeWinner(best_util))
                    break;
                if (results.size() > 1 && isSignificantlyBetter(results[0].utility, results[1].utility))
                    break;
            }

        } while (!timer.isTimeOut() && hEvalUsed);

        return {results[0].action, results[0].utility};
    }


    std::string getMetrics() const {
        return metrics.toString();
    }
};

#endif // MTD_H