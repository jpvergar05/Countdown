/*
 * File: Game.cpp
 * Description: Implementation of the Game class. Includes methods for initializing
 *              the game, performing calculations, solving the puzzle, and managing the
 *              undo stack.
 *
 *
 * Notes:
 * - Supports operations with integer inputs only.
 * - Division by zero is handled safely within performOperation.
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional> 
#include <cmath>
#include "Game.h"
#include "mersenne-twister.h"

using namespace std;

// Constructor to initialize the game state
// Chooses random numbers and the target (goal)
// Parameter numLarge is the number (0-4) of large numbers
// Remaining of random numbers will be small
Game::Game(int numLarge) {
	// Sets the random numbers
	for (int i = 0; i < NUMBERS_COUNT; i++) {
		if (i < numLarge) {
			numbers[i] = chooseRandomNumber(1, 4) * 25;
		}
		else {
			numbers[i] = chooseRandomNumber(1, 10);
		}
	}

	// Sets the target
	target = chooseRandomNumber(101, 999);
}

// Display numbers left for the users to choose and the target
void Game::displayNumbers() {
	cout << "Your numbers are: ";

	// Display all numbers remaining (skip 0s which indicate an open spot)
	for (int i = 0; i < NUMBERS_COUNT; i++) {
		if (numbers[i] != 0) {
			cout << numbers[i] << " ";
		}
	}
	cout << endl;

	cout << "The target is: " << target << endl;
}

// Add a number to the numbers available for the user to choose
// Parameter addNum is the number to add
void Game::addNumber(int addNum) {
	// Go through the available spots and put the new number at the first 0 (empty spot)
	for (int i = 0; i < NUMBERS_COUNT; i++) {
		if (numbers[i] == 0) {
			numbers[i] = addNum;
			return;
		}
	}

	cout << "Error: no space for added number!" << endl;
}

// Determine if the user has lost the game
// Returns true if the user has lost, falst if they have not (they've won or game is still going)
bool Game::lostGame() {
	// Winning takes priority--not a loss if the game has been won
	if (wonGame()) {
		return false;
	}

	// Look through each number, count the numbers that remain (aren't 0)
	int countNumbersRemain = 0;
	for (int i = 0; i < NUMBERS_COUNT; i++) {
		if (numbers[i] != 0) {
			countNumbersRemain++;
		}
	}

	// If there is 1 or fewer number left, the user has lost
	return (countNumbersRemain <= 1);
}

// Removes the given number from the list of numbers to work with
// userNumber is the number to remove
// Returns true if the userNumber was found (and removed)
// Returns false if userNumber is not in the list
bool Game::removeNumber(int userNumber) {
	for (int i = 0; i < NUMBERS_COUNT; i++) {
		if (numbers[i] == userNumber) {
			numbers[i] = 0;
			// Return true, number was false
			return true;
		}
	}

	// Number wasn't found, return false
	return false;
}

// Calculates the answer of the specified operation
// operation is a character, either '+', '-', '*', or '/' to specify the type of calculation
// num1 and num2 are the two numbers (where order matters, num1 comes first, e.g. num1 - num2)
// Returns the result of the calculation
int Game::performOperation(int num1, char operation, int num2) {
	// Perform the calculation depending on the operation chosen
	// (Note: breaks not needed since return statement leaves the function)
	switch (operation) {
	case '+':
		return num1 + num2;
	case '-':
		return num1 - num2;
	case '/':
		return num1 / num2;
	case '*':
		return num1 * num2;
	}

	// Error: invalid operation, return 0
	return 0;
}

// Determines if the user has won the game
// Returns true if user has won, false if they have lost or game still going
bool Game::wonGame() {
	// Look through available numbers
	for (int i = 0; i < NUMBERS_COUNT; i++) {
		// If target is found, return true
		if (numbers[i] == target) {
			return true;
		}
	}

	// Target not found
	return false;
}

// Determines if the game is over
// Returns true if game is over (user has either won or lost)
// Returns false if game still going
bool Game::gameOver() {
	return wonGame() || lostGame();
}

bool Game::solve(std::vector<std::string>& steps) {
    // Create a copy of the current numbers array for solving
    int tempNumbers[NUMBERS_COUNT];
    for (int i = 0; i < NUMBERS_COUNT; ++i) {
        tempNumbers[i] = numbers[i];
    }

    // Helper function to calculate outcomes based on the operation
    auto performOperations = [&](int a, int b, char op) -> double {
        if (op == '+') return a + b;
        if (op == '-') return a - b;
        if (op == '*') return a * b;
        if (op == '/' && b != 0) return static_cast<double>(a) / b; // Allow fractional division
        return -1; // Invalid operation
    };

    // Define operation priority
    const std::vector<char> operationPriority = {'+', '-', '*', '/'};

    // Recursively explore solutions
    std::function<bool()> findSolution = [&]() {
        // Check if the target is reached
        for (int i = 0; i < NUMBERS_COUNT; ++i) {
            if (static_cast<int>(tempNumbers[i]) == target) { // Compare as integer
                return true; // Solution found
            }
        }

        // Try pairs of numbers and operations in priority order
        for (int i = 0; i < NUMBERS_COUNT; ++i) {
            for (int j = 0; j < NUMBERS_COUNT; ++j) {
                if (i == j || tempNumbers[i] == 0 || tempNumbers[j] == 0) continue; // Skip invalid pairs

                double a = tempNumbers[i], b = tempNumbers[j];

                // Enforce operation priority
                for (char op : operationPriority) {
                    double result = performOperations(a, b, op);
                    if (result == -1 || std::isnan(result)) continue; // Skip invalid results

                    // Log the step
                    steps.push_back(to_string(static_cast<int>(a)) + " " + op + " " +
                                    to_string(static_cast<int>(b)) + " = " + to_string(static_cast<int>(result)));

                    // Update numbers array
                    tempNumbers[i] = result;
                    tempNumbers[j] = 0;

                    // Recursive attempt
                    if (findSolution()) {
                        return true;
                    }

                    // Backtrack: undo the operation
                    steps.pop_back();
                    tempNumbers[i] = a;
                    tempNumbers[j] = b;
                }
            }
        }
        return false; // No solution found
    };

    return findSolution(); // Begin solving
}

// Returns the current game state, including the numbers array and target value
Game::GameState Game::getCurrentState() const {
    GameState state; // Create a new GameState object to store the current state
    for (int i = 0; i < NUMBERS_COUNT; ++i) {
        state.numbers[i] = numbers[i]; // Copy each number from the current game to the state object
    }
    state.target = target; // Store the target value in the state object
    return state; // Return the populated GameState object
}

// Restores the game state from a provided GameState object
void Game::restoreState(const GameState& state) {
    for (int i = 0; i < NUMBERS_COUNT; ++i) {
        numbers[i] = state.numbers[i]; // Copy each number from the state object to the current game
    }
    target = state.target; // Restore the target value from the state object
}

// Saves the current game state to the undo stack
void Game::pushUndoState() {
    // Get the current game state
    auto currentState = getCurrentState();
    // Create a new UndoNode with the current state and link it to the existing undo stack
    undoStack = std::make_unique<UndoNode>(currentState, std::move(undoStack));
}

// Restores the last saved game state from the undo stack, if available
bool Game::popUndoState() {
    if (!undoStack) {
        return false; // No state to undo; return false
    }
    restoreState(undoStack->state); // Restore the state from the top of the stack
    undoStack = std::move(undoStack->next); // Remove the top node from the stack
    return true; // Indicate success
}

// Clears all saved states from the undo stack
void Game::clearUndoStack() {
    undoStack.reset(); // Releases all nodes
}


