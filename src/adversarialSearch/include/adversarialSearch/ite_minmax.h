// ite_minmax.h

#include <set>

#include "vgame.h"
#include "utilities.h"

#ifndef ITEMINMAX_H
#define ITEMINMAX_H

using namespace std;

template <class S, class A, class P, class U>
class ite_minmax {
private:
    const VGame<S, A, P, U>& game;
    bool hEvalUsed;
    int currentDepthLimit;
    int startDepthLimit;
    Timer timer;
    SimpleMetrics metrics;

    void inline updateMetrics(int depth) {
        #ifdef ENABLE_METRICS
            metrics.updateMaxDepth(depth);
            metrics.incrementNodesExpanded();
        #endif
    }

protected:
    virtual U maxValue(S state, P player, U alpha, U beta, int depth) {
        updateMetrics(depth);
    
        if (game.isTerminal(state) || depth == 0 || timer.isTimeOut())
            return eval(state, player);
        
        auto value = game.util_min;
    
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
      
            auto newState = game.getResult(state, action);
            value = max(value, minValue(newState, player, alpha, beta, depth - 1));
            
            if (value >= beta)
                return value;
            alpha = max(alpha, value);
        }
        return value;
    }


    virtual U minValue(S state, P player, U alpha, U beta, int depth) {
        updateMetrics(depth);
    
        if (game.isTerminal(state) || depth == 0 || timer.isTimeOut())
            return eval(state, player);
        
        auto value = game.util_max;
    
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
            auto newState = game.getResult(state, action);
            value = min(value, maxValue(newState, player, alpha, beta, depth - 1));
            if (value <= alpha)
                return value;
            beta = min(beta, value);
        }
        return value;
    }


    virtual void incrementDepthLimit() {
        this->currentDepthLimit++;
    }

    virtual bool isSignificantlyBetter(U newUtility, U utility) {
        return false;
    }

    virtual bool hasSafeWinner(U resultUtility) {
        return resultUtility <= game.util_min || resultUtility >= game.util_max;
    }

    virtual U eval(S state, P player) {
        hEvalUsed = true;
        return game.getUtility(state, player);
    }

    virtual vector<A> orderActions(S state, vector<A> actions, P player, int depth) {
        return actions;
    }


public:

    // Constructor
    ite_minmax(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
    : game(game), startDepthLimit(startDepth), timer(maxTimeSeconds)
    {}
    
    virtual pair<A, U> makeDecision(S state) {
        metrics.reset();    
        timer.start();
        currentDepthLimit = startDepthLimit;
        auto player = game.getPlayer(state);
    
        auto actions = orderActions(state, game.getActions(state), player, currentDepthLimit);
        vector<actionUtility<A, U>> results;
        for (auto action : actions)
            results.push_back({action, game.util_min});

        do {
            incrementDepthLimit();
            hEvalUsed = false;
            
            for (auto actUtil : results) {
                auto newState = game.getResult(state, actUtil.action);
                auto utility = minValue(newState, player, game.util_min, game.util_max, currentDepthLimit);
                
                if (!timer.isTimeOut()) {
                    actUtil.utility = utility;
                    actUtil.completed = true;
                }
                else
                    actUtil.completed = false; // incomplete 
            }

            // Sort the results and update only if the timer is not timed out
            // or if the first two results are completed
            // else use the previous results
            if (!timer.isTimeOut() || (new_results[0].completed && new_results[1].completed))
            {
                // Remove incomplete results
                if (timer.isTimeOut()) {
                    int i;
                    for (i = 0; i < results.size(); i++) {
                        if (!results[i].completed)
                            break;
                    }
                    results.resize(i);
                }
                std::sort(results.begin(), results.end());
            }
    
            if (!timer.isTimeOut()) {
                if (hasSafeWinner(results.begin()->utility))
                    break;
                else if (results.size() > 1 && isSignificantlyBetter(results.begin()->utility, results.rbegin()->utility))
                    break;
            }
            
        } while (!timer.isTimeOut() && hEvalUsed);
        
        return {results.begin()->action, results.begin()->utility};
    }

    std::string getMetrics() const {
        return metrics.toString();
    }
};

#endif // ITEMINMAX_H