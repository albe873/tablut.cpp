// IntDeepAlphaBetaSearch.h

#include <set>
#include <omp.h>

#include "vgame.h"
#include "utilities.h"

using namespace std;

template <class S, class A, class P, class U>
class parIteDABSearch {
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
    parIteDABSearch(const VGame<S, A, P, U>& game, U utilMin, U utilMax, int startDepth, int maxTimeSeconds)
    : game(game), utilMin(utilMin), utilMax(utilMax), currentDepthLimit(startDepth), timer(maxTimeSeconds)
    {}
    
    A makeDecision(S state) {
        metrics.reset();    
        this->timer.start();
        auto player = game.getPlayer(state);
    
        auto actions = orderActions(state, game.getActions(state), player, 0);
        vector<actionUtility<A, U>> results;
        for (auto action : actions)
            results.push_back({action, utilMin});
        
        do {
            incrementDepthLimit();
            hEvalUsed = false;
    
            vector<actionUtility<A, U>> newResults;
            
            #pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < results.size(); i++) {
                auto actUtil = results[i];
                auto newState = game.getResult(state, actUtil.action);
                actUtil.utility = minValue(newState, player, utilMin, utilMax, 1);
                
                // no break allowed, so we add the result only if the timer is not timeout
                // we need to collect all minValue results (that terminate early if timeout) and discard the values
                if (!timer.isTimeOut()) {
                    #pragma omp critical
                    {newResults.push_back(actUtil);}
                }
            }

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
        
        return results[0].action;
    }

    std::string getMetrics() const {
        return metrics.toString();
    }
};