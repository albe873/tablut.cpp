// vgame.h

#ifndef IGAME_H
#define IGAME_H

#include <vector>

template <typename S, typename A, typename P, typename U>
class VGame {
public:

    virtual S getInitialState() const = 0;

    virtual std::vector<P> getPlayers() const = 0;
    
    virtual P getPlayer(const S&) const = 0;
    
    virtual std::vector<A> getActions(const S&) const = 0;
    
    virtual S getResult(S, const A&) const = 0;
    
    virtual bool isTerminal(const S&) const = 0;
    
    virtual U getUtility(const S&, const P&) const = 0;
    
    virtual ~VGame() = default;
};

#endif