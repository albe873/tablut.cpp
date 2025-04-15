// common.h

#include <cstdint>
#include <vector>
#include <string>

#ifndef COMMON_H
#define COMMON_H

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
    // --- Constructor ---
    Move();
    Move(cord from, cord to);
    
    // --- Getters ---
    cord getFrom() const;
    cord getTo() const;

    // --- Utilities ---
    static cord calculateNewCord(const cord& start, const cord& direction);
    std::string toString();

    // --- Operators ---
    bool operator==(const Move& other) const;
    bool equals(const Move& other) const;
};

#endif // COMMON_H