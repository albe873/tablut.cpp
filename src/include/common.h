// common.h
#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <vector>

struct cord {
    int8_t x;
    int8_t y;

    cord();
    cord(int8_t x, int8_t y);

    bool operator==(const cord& other) const;
};

class Directions {
public:
    static const cord UP;
    static const cord DOWN;
    static const cord LEFT;
    static const cord RIGHT;

    static const std::vector<cord> ALL_DIRECTIONS;
};

class Move {
private:
    cord from;
    cord to;
public:
    Move();
    Move(cord from, cord to);
    cord getFrom() const;
    cord getTo() const;
    static cord calculateNewCord(cord& start, cord& direction);
};

#endif // COMMON_H