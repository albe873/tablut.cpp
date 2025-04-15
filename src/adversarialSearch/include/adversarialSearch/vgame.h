// vgame.h

#ifndef IGAME_H
#define IGAME_H

#include <vector>

template <typename STATE, typename ACTION, typename PLAYER, typename UTILITY>
class VGame {
public:

    virtual STATE getInitialState() const = 0;

    virtual std::vector<PLAYER> getPlayers() const = 0;
    
    virtual PLAYER getPlayer(const STATE&) const = 0;
    
    virtual std::vector<ACTION> getActions(const STATE&) const = 0;
    
    virtual STATE getResult(STATE, const ACTION&) const = 0;
    
    virtual bool isTerminal(const STATE&) const = 0;
    
    virtual UTILITY getUtility(const STATE&, const PLAYER&) const = 0;
    
    virtual ~VGame() = default;
};

#endif