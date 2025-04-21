// IntDeepAlphaBetaSearch.h

#include <set>
#include <omp.h>
#include <algorithm>

#include "vgame.h"
#include "utilities.h"

#ifndef PARITEDEEPABSEARCH_H
#define PARITEDEEPABSEARCH_H

using namespace std;

// Metrics are disabled by default
// they can be enabled by defining ENABLE_METRICS in this file: #define ENABLE_METRICS
// or (preferred way) by passing -DENABLE_METRICS to the compiler

template <typename S, typename A, typename P, typename U>
class parIteDABSearch {
private:
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

    U maxValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(currentDepthLimit);

        if (game.isTerminal(state)) {
            auto value = eval(state, player);
            return evalTerminal(value, player, currentDepthLimit - depth);
        }

        if (depth == 0 || timer.isTimeOut())
            return eval(state, player);
        
        auto value = game.util_min;
    
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
      
            auto newState = game.getResult(state, action);
            value = max(value, minValue(newState, player, alpha, beta, depth - 1));
            
            if (value >= beta)
                break;
            
            alpha = max(alpha, value);
        }
        return value;
    }


    U minValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(currentDepthLimit);

        if (game.isTerminal(state)) {
            auto value = eval(state, player);
            return evalTerminal(value, player, currentDepthLimit - depth);
        }
    
        if (depth == 0 || timer.isTimeOut())
            return eval(state, player);
        
        auto value = game.util_max;
        
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {

            auto newState = game.getResult(state, action);
            value = min(value, maxValue(newState, player, alpha, beta, depth - 1));
            
            if (value <= alpha)
                break;
            
            beta = min(beta, value);
        }
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
        return value;
    }

    virtual vector<A> orderActions(const S& state, vector<A> actions, const P& player, const int& depth) {
        return actions;
    }


public:

    // Constructor
    parIteDABSearch(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
    : game(game), startDepthLimit(startDepth), timer(maxTimeSeconds)
    {}
    
    pair<A, U> makeDecision(S state) {
        metrics.reset();
        currentDepthLimit = startDepthLimit;
        timer.start();
        auto player = game.getPlayer(state);
    
        auto actions = orderActions(state, game.getActions(state), player, 0);
        vector<actionUtility<A, U>> results;
        for (auto action : actions)
            results.push_back({action, game.util_min});
        
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
                    maxI = max(i, maxI);
                    actUtil.utility = newUtil;
                }
                
                if ( i <= maxI) {
                    #pragma omp critical
                    {
                        newResults[i] = actUtil;
                    }
                }
            }

            // remove unprocessed results
            newResults.resize(maxI + 1);

            // Sort the results
            sort(newResults.begin(), newResults.end());
    
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

#endif // PARITEDEEPABSEARCH_H