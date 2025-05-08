// common.cpp

#include "tablut/common.h"

// cord
cord::cord() : x(0), y(0) {}
cord::cord(int x, int y) : x(x), y(y) {}
bool cord::operator==(const cord& other) const {
    return x == other.x && y == other.y;
}

// Directions
const cord Directions::UP = {0, -1};
const cord Directions::DOWN = {0, 1};
const cord Directions::LEFT = {-1, 0};
const cord Directions::RIGHT = {1, 0};
const std::vector<cord> Directions::ALL_DIRECTIONS = {UP, DOWN, LEFT, RIGHT};


// Move
// --- Constructor ---
Move::Move() = default;
Move::Move(cord from, cord to) : from(from), to(to) {}

// --- Getters ---
cord Move::getFrom() const { return from; }
cord Move::getTo() const { return to; }

// --- Utilities ---
cord Move::calculateNewCord(const cord& start, const cord& direction) {
    return cord(start.x + direction.x, start.y + direction.y);
}

std::string Move::toString() const {
    return "Move from (" + std::to_string(from.x) + ", " + std::to_string(from.y) + 
        ") to (" + std::to_string(to.x) + ", " + std::to_string(to.y) + ")";
}

// --- Operators ---
bool Move::operator==(const Move& other) const {
    return from == other.from && to == other.to;
}
bool Move::equals(const Move& other) const {
    return *this == other;
}