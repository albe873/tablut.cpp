#ifndef IGAME_H
#define IGAME_H

#include <vector>

template <typename S, typename A, typename P, typename U>
class VGame {
public:

    virtual S getInitialState() = 0;

    virtual std::vector<P> getPlayers() = 0;
    
    virtual P getPlayer(S state) = 0;
    
    virtual std::vector<A> getActions(S state) = 0;
    
    virtual S getResult(S state, A action) = 0;
    
    virtual bool isTerminal(S state) = 0;
    
    virtual U getUtility(S state) = 0;
    
    virtual ~VGame() = default;
};

#endif