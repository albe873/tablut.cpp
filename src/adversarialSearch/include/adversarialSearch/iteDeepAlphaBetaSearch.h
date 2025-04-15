// IntDeepAlphaBetaSearch.h

#include <set>

#include "vgame.h"
#include "utilities.h"

#ifndef INTDEEPABSEARCH_H
#define INTDEEPABSEARCH_H

using namespace std;

template <class S, class A, class P, class U>
class IteDeepAlphaBetaSearch {
private:
    const VGame<S, A, P, U>& game;
    U utilMax, utilMin;
    bool hEvalUsed;
    int currentDepthLimit;
    Timer timer;
    SimpleMetrics metrics;

    U maxValue(S state, P player, U alpha, U beta, int depth) {
        updateMetrics(depth);
    
        if (game.isTerminal(state) || depth >= currentDepthLimit || timer.isTimeOut())
            return eval(state, player);
        
        auto value = utilMin;
    
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
      
            auto newState = game.getResult(state, action);
            value = max(value, minValue(newState, player, alpha, beta, depth + 1));
            
            if (value >= beta)
                return value;
            alpha = max(alpha, value);
        }
        return value;
    }


    U minValue(S state, P player, U alpha, U beta, int depth) {
        updateMetrics(depth);
    
        if (game.isTerminal(state) || depth >= currentDepthLimit || timer.isTimeOut())
            return eval(state, player);
        
        auto value = utilMax;
    
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
            auto newState = game.getResult(state, action);
            value = min(value, maxValue(newState, player, alpha, beta, depth + 1));
            if (value <= alpha)
                return value;
            beta = min(beta, value);
        }
        return value;
    }


protected:
    void incrementDepthLimit() {
        this->currentDepthLimit++;
    }

    bool isSignificantlyBetter(U newUtility, U utility) {
        return false;
    }

    bool hasSafeWinner(U resultUtility) {
        return resultUtility <= utilMin || resultUtility >= utilMax;
    }

    U eval(S state, P player) {
        hEvalUsed = true;
        return game.getUtility(state, player);
    }

    vector<A> orderActions(S state, vector<A> actions, P player, int depth) {
        return actions;
    }

    void updateMetrics(int depth) {
        metrics.updateMaxDepth(depth);
        metrics.incrementNodesExpanded();
    }


public:

    // Constructor
    IteDeepAlphaBetaSearch(const VGame<S, A, P, U>& game, U utilMin, U utilMax, int maxTimeSeconds)
    : game(game), utilMin(utilMin), utilMax(utilMax), currentDepthLimit(0), timer(maxTimeSeconds)
    {}
    
    A makeDecision(S state) {
        metrics.reset();    
        this->timer.start();
        auto player = game.getPlayer(state);
    
        auto actions = orderActions(state, game.getActions(state), player, 0);
        multiset<actionUtility<A, U>> results;
        for (auto action : actions)
            results.insert({action, utilMin});
        
        do {
            incrementDepthLimit();
            hEvalUsed = false;
    
            multiset<actionUtility<A, U>> newResults;
            
            for (auto actUtil : results) {
                auto newState = game.getResult(state, actUtil.action);
                actUtil.utility = minValue(newState, player, utilMin, utilMax, 1);
    
                newResults.insert(actUtil);
                
                if (timer.isTimeOut())
                    break;
            }
    
            if (!newResults.empty()) {
                results = newResults;
                if (!timer.isTimeOut()) {
                    if (hasSafeWinner(results.begin()->utility))
                        break;
                    else if (results.size() > 1 && isSignificantlyBetter(results.begin()->utility, results.rbegin()->utility))
                        break;
                }
            }
            
        } while (!timer.isTimeOut() && hEvalUsed);
        
        return results.begin()->action;
    }

    std::string getMetrics() const {
        return metrics.toString();
    }
};

#endif // INTDEEPABSEARCH_H