#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

class GameI {
public:
    virtual ~GameI() = default;

    // Initialize the game
    virtual void initialize() = 0;

    // Start the game loop
    virtual void start() = 0;

    // Process a single turn
    virtual void processTurn() = 0;

    // Check if the game is over
    virtual bool isGameOver() const = 0;

    // Display the current game state
    virtual void displayState() const = 0;
};

#endif // GAME_INTERFACE_H