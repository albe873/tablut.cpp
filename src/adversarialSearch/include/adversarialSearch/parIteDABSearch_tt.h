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
    Timer timer;
    SimpleMetrics metrics;
    t_table<U, A> table;

    U maxValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(depth);

        if (game.isTerminal(state)) {
            auto value = eval(state, player);
            return evalTerminal(value, player, depth);
        }

        if (depth >= currentDepthLimit || timer.isTimeOut())
            return eval(state, player);
        
        // Check transposition table
        auto hash = state.hash64();
        auto value = table.probe(hash, alpha, beta, depth);
        if (value != game.util_unknown) {
            updateHit();
            return value;
        }
        updateMiss();
        
        value = game.util_min;
        auto originalAlpha = alpha; // save original alpha value

        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
      
            auto newState = game.getResult(state, action);
            
            // recursive call
            value = max(value, minValue(newState, player, alpha, beta, depth + 1));
            
            // cutoff
            if (value >= beta)
                break;
            
            // update alpha
            alpha = max(alpha, value);
        }

        // transposition table insert, fail high result implies a lower bound
        if (value >= beta) 
            table.insert(hash, entry_type::l_bound, value, depth);

        return value;
    }


    U minValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(depth);

        if (game.isTerminal(state)) {
            auto value = eval(state, player);
            return evalTerminal(value, player, depth);
        }
    
        if (depth >= currentDepthLimit || timer.isTimeOut())
            return eval(state, player);
        
        // Check transposition table
        auto hash = state.hash64();
        auto value = table.probe(hash, alpha, beta, depth);
        if (value != game.util_unknown) {
            updateHit();
            return value;
        }
        updateMiss();

        value = game.util_max;
        
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
            auto newState = game.getResult(state, action);
            value = min(value, maxValue(newState, player, alpha, beta, depth + 1));
            
            if (value <= alpha)
                break;

            beta = min(beta, value);
        }

        // transposition table insert, fail low result implies an upper bound
        if (value <= alpha) 
            table.insert(hash, entry_type::u_bound, value, depth);
        
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

    virtual U evalTerminal(U value, const P& player, const int& depth) {
        hEvalUsed = true;
        return value;
    }

    virtual vector<A> orderActions(const S& state, const vector<A>& actions, const P& player, const int& depth) {
        return actions;
    }


public:

    // Constructor
    parIteDABSearch_tt(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
    : game(game), currentDepthLimit(startDepth), timer(maxTimeSeconds),  table(game.util_unknown)
    {}
    
    pair<A, U> makeDecision(S state) {
        metrics.reset();   
        this->timer.start();
        auto player = game.getPlayer(state);
    
        auto actions = orderActions(state, game.getActions(state), player, 0);
        vector<actionUtility<A, U>> results;
        for (auto action : actions)
            results.push_back({action, game.util_min});
        
        do {
            incrementDepthLimit();
            table.clear(); 
            hEvalUsed = false;
    
            vector<actionUtility<A, U>> newResults;
            newResults.resize(results.size());
            
            int maxI = 0;
            
            #pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < results.size(); i++) {
                auto actUtil = results[i];
                auto newState = game.getResult(state, actUtil.action);
                auto newUtil = minValue(newState, player, game.util_min, game.util_max, 1);
                
                // no break allowed, so we add the first results calculated up to the timeout
                // some threads might finish earlier than other with a smaller i, so I need to add
                // up to maxI results in the newResults (some might be with a smaller) with the previous
                // utility (if timeOut, the new utility is calculated at a smaller depth) 
                if (!timer.isTimeOut()) {
                    maxI = max(i, maxI);
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