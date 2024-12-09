/*
 * File: Game.h
 * Description: Header file for the Game class, which manages game state, operations,
 *              and undo functionality for the Countdown Game.
 *
 *
 * Notes:
 * - Ensure proper stack management for the undo feature.
 * - All operations (addition, subtraction, etc.) are validated within the class.
 * - Linked list
 */


#pragma once
#include <vector>
#include <string>
#include <memory>

// Game class keeps track of the state of a single game
class Game {
public:
    // Quantity of numbers that are part of the game
    static const int NUMBERS_COUNT = 6;

private:
    // Array of numbers (0 if a spot is empty)
    int numbers[NUMBERS_COUNT];

    // Target (goal the user is going for)
    int target;

    // Struct to store a snapshot of the game state
    struct GameState {
        int numbers[NUMBERS_COUNT];
        int target;
    };

    // Struct for a node in the undo stack (linked list)
    struct UndoNode {
        GameState state;  // Snapshot of the game state
        std::unique_ptr<UndoNode> next; // Pointer to the next node in the stack

        // Constructor to initialize a node with a given state and next pointer
        UndoNode(const GameState& gameState, std::unique_ptr<UndoNode> nextNode = nullptr)
            : state(gameState), next(std::move(nextNode)) {}
    };

    std::unique_ptr<UndoNode> undoStack; // Pointer to the top of the undo stack

    // Helper Methods
    GameState getCurrentState() const;         // Capture the current game state
    void restoreState(const GameState& state); // Restore the game to a specific state

public:
    // Constructor
    Game(int numLarge);

    // Core functionality
    void displayNumbers();
    void addNumber(int num);
    bool wonGame();
    bool lostGame();
    bool gameOver();
    bool removeNumber(int userNumber);
    int performOperation(int num1, char operation, int num2);

    // Solve functionality
    bool solve(std::vector<std::string>& steps);

    // Undo functionality
    void pushUndoState();   // Save current state to undo stack
    bool popUndoState();    // Restore the last state from undo stack
    void clearUndoStack(); // Clear the undo stack
};
