// IntDeepAlphaBetaSearch.h

#include <set>
#include <omp.h>
#include <algorithm>

#include "vgame.h"
#include "utilities.h"
#include "t_table.h"

#ifndef PARITEDEEPABSEARCHTT_H
#define PARITEDEEPABSEARCHTT_H

using namespace std;


// Metrics are disabled by default
// they can be enabled by defining ENABLE_METRICS in this file: #define ENABLE_METRICS
// or (preferred way) by passing -DENABLE_METRICS to the compiler

template <typename S, typename A, typename P, typename U>
class parIteDABSearch_tt {
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

protected:
    const VGame<S, A, P, U>& game;
    bool hEvalUsed;
    int currentDepthLimit;
    int startDepthLimit;
    Timer timer;
    SimpleMetrics metrics;
    t_table<U, A> table;

    U maxValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(currentDepthLimit);

        if (game.isTerminal(state)) {
            auto value = eval(state, player);
            return evalTerminal(value, player, currentDepthLimit - depth);
        }

        if (depth == 0 || timer.isTimeOut())
            return eval(state, player);
        
        // Check transposition table
        int best_action_index = 0;
        auto hash = state.hash64();
        auto value = table.probe(hash, alpha, beta, depth, best_action_index);
        if (value != game.util_unknown) {
            updateHit();
            return value;
        }
        updateMiss();
        
        value = game.util_min;

        auto actions = orderActions(state, game.getActions(state), player, depth, best_action_index);

        for (int i = 0; i < actions.size(); i++) {
            auto action = actions[i];
      
            auto newState = game.getResult(state, action);
            
            // recursive call
            auto min_value = minValue(newState, player, alpha, beta, depth - 1);

            // update value with max value (equivalent to max(value, minValue))
            if (min_value > value) {
                value = min_value;
                best_action_index = i;
            }
            
            // cutoff
            if (value >= beta)
                break;
            
            // update alpha
            alpha = max(alpha, value);
        }

        // transposition table insert, fail high result implies a lower bound, else exact
        if (value >= beta) 
            table.insert(hash, entry_type::l_bound, value, depth);
        else
            table.insert(hash, entry_type::exact, value, depth, best_action_index);

        return value;
    }


    U minValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(currentDepthLimit - depth);

        if (game.isTerminal(state)) {
            auto value = eval(state, player);
            return evalTerminal(value, player, currentDepthLimit - depth);
        }
    
        if (depth == 0 || timer.isTimeOut())
            return eval(state, player);
        
        // Check transposition table
        int best_action_index = 0;
        auto hash = state.hash64();
        auto value = table.probe(hash, alpha, beta, depth, best_action_index);
        if (value != game.util_unknown) {
            updateHit();
            return value;
        }
        updateMiss();

        value = game.util_max;
        
        auto actions = orderActions(state, game.getActions(state), player, depth, best_action_index);

        for (int i = 0; i < actions.size(); i++) {
            auto action = actions[i];

            auto newState = game.getResult(state, action);

            auto max_value = maxValue(newState, player, alpha, beta, depth - 1);

            // update value with min value (equivalent to min(value, maxValue))
            if (max_value < value) {
                value = max_value;
                best_action_index = i;
            }
            
            if (value <= alpha)
                break;

            beta = min(beta, value);
        }

        // transposition table insert, fail low result implies an upper bound, else exact
        if (value <= alpha) 
            table.insert(hash, entry_type::u_bound, value, depth);
        else
            table.insert(hash, entry_type::exact, value, depth, best_action_index);
        
        return value;
    }

    virtual void incrementDepthLimit() {
        this->currentDepthLimit++;
    }

    virtual bool isSignificantlyBetter(const U& newUtility, const U& utility) {
        return false;
    }

    virtual bool hasSafeWinner(const U& resultUtility) {
        return resultUtility <= game.util_min || resultUtility >= game.util_max;
    }

    virtual U eval(const S& state, const P& player) {
        hEvalUsed = true;
        return game.getUtility(state, player);
    }

    virtual U evalTerminal(U value, const P& player, const int& distance) {
        hEvalUsed = true;
        return value;
    }

    virtual vector<A> orderActions(const S& state, vector<A> actions, 
                                   const P& player, const int& depth, const int& best_action_hint) {
        return actions;
    }


public:

    // Constructor
    parIteDABSearch_tt(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
    : game(game), startDepthLimit(startDepth), timer(maxTimeSeconds),  table(game.util_unknown)
    {}
    
    pair<A, U> makeDecision(S state) {
        // reset the metrics
        metrics.reset();

        // start the timer
        this->timer.start();

        // reset depth limit
        currentDepthLimit = startDepthLimit;
        
        // get player
        auto player = game.getPlayer(state);
    
        // get actions and put them in results array 
        auto actions = orderActions(state, game.getActions(state), player, 0, 0);
        vector<actionUtility<A, U>> results;
        for (auto action : actions)
            results.push_back({action, game.util_min});
        
        // iterative deepening main loop
        do {
            incrementDepthLimit();
            hEvalUsed = false;
    
            vector<actionUtility<A, U>> newResults;
            newResults.resize(results.size());
            
            int maxI = 0;
            
            #pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < results.size(); i++) {
                auto actUtil = results[i];
                auto newState = game.getResult(state, actUtil.action);
                auto newUtil = minValue(newState, player, game.util_min, game.util_max, currentDepthLimit);
                
                // no break allowed, so we add the first results calculated up to the timeout
                // some threads might finish earlier than other with a smaller i, so I need to add
                // up to maxI results in the newResults (some might be with a smaller) with the previous
                // utility (if timeOut, the new utility is calculated at a smaller depth) 
                if (!timer.isTimeOut()) {
                    #pragma omp critical
                    {
                        maxI = max(i, maxI);
                    }
                    actUtil.utility = newUtil;
                }
                
                #pragma omp critical
                {
                    newResults[i] = actUtil;
                }
            }

            newResults.resize(maxI + 1);

            // Sort the results
            std::sort(newResults.begin(), newResults.end());
    
            if (!newResults.empty()) {
                results = newResults;
                if (!timer.isTimeOut()) {
                    if (hasSafeWinner(results[0].utility))
                        break;
                    else if (results.size() > 1 && isSignificantlyBetter(results[0].utility, results[0].utility))
                        break;
                }
            }
            
        } while (!timer.isTimeOut() && hEvalUsed);
        
        return {results[0].action, results[0].utility};
    }

    std::string getMetrics() const {
        return metrics.toString();
    }
};

#endif // PARITEDEEPABSEARCHTT_H