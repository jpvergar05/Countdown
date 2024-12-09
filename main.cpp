/*
 * File: main.cpp
 * Description: Serves as the entry point for the Countdown Game. Facilitates the gameplay 
 *              loop by interacting with the user, initializing the game, and managing 
 *              user inputs for calculations, solving, and undo functionality. 
 *              Also handles game setup, displays results, and allows replay.
 *
 * Author: <JP Vergar>
 * Date: <12/06/24>
 *
 * Notes:
 * - Leverages the Mersenne Twister random number generator to ensure consistent random 
 *   number generation across games.
 * - Implements features like undo (using a stack-based mechanism), solving the puzzle, 
 *   and validating user inputs for calculations.
 * - Ensures that the game continues until the user opts to quit or a win/lose condition 
 *   is met, with an option to replay after each game.
*/

#include <iostream>
#include <string>
#include <sstream>
#include "mersenne-twister.h"
#include "Game.h"

using namespace std;

int promptAndGetNumLarge();
void processCalculation(string userInput, int& num1, char& operation, int& num2);

int main() {
    // Input a random seed for the program, and call the seed function
    int randSeed;
    cout << "Enter random seed: " << endl;
    cin >> randSeed;
    seed(randSeed);

    // userContinue is used to input whether the user wants to continue to play another game
    string userContinue;
    do {
        // Get the quantity of large numbers
        int numLarge = promptAndGetNumLarge();
        

        // Clear the undo stack when starting a new game
        Game game(numLarge);
        game.pushUndoState(); // Save the initial board state
        game.clearUndoStack();

        // Clear newline from previous input
        string userInput;
        getline(cin, userInput);

        // Repeat until the game is over
        while (!game.gameOver()) {
            game.displayNumbers();
            cout << "Enter a calculation (or U to Undo, S to Solve, or Q to Quit): " << endl;
            getline(cin, userInput);

            // Quit this game if the user chooses Q
            if (userInput == "q" || userInput == "Q") {
                cout << "Exiting this game." << endl;
                break;
            }

            // Solve the game if the user chooses S
            if (userInput == "S" || userInput == "s") {
                vector<string> steps;
                if (game.solve(steps)) {
                    cout << "Solution:" << endl;
                    for (const string& step : steps) {
                        cout << step << endl;
                    }
                } else {
                    cout << "Unable to find a solution." << endl;
                }
                continue; // Continue to let the user play
            }

            // Handle Undo
            if (userInput == "U" || userInput == "u") {
                if (!game.popUndoState()) {
                    cout << "Cannot undo." << endl;
                } 
                continue; // Skip the rest of the loop
            }

            // Process the user's calculation input
            int num1;
            char operation;
            int num2;
            processCalculation(userInput, num1, operation, num2);

            // Save the current state before performing the operation
            game.pushUndoState();

            // Validate and execute the user's calculation
            if (!game.removeNumber(num1)) {
                cout << num1 << " is not a valid number." << endl;
            } else if (!game.removeNumber(num2)) {
                cout << num2 << " is not a valid number." << endl;
                game.addNumber(num1); // Restore the first number
            } else {
                int result = game.performOperation(num1, operation, num2);
                cout << num1 << " " << operation << " " << num2 << " = " << result << endl << endl;
                game.addNumber(result);

                // Display win or lose messages if appropriate
                if (game.wonGame()) {
                    cout << "You win!" << endl;
                } else if (game.lostGame()) {
                    cout << "You lose." << endl;
                }
            }
        }

        // Ask user if they want to continue, do so if they type Y or y
        cout << endl;
        cout << "Would you like to play again (y/n)? ";
        getline(cin, userContinue);
    } while (userContinue == "y" || userContinue == "Y");
}

// Display the greeting and the prompt for choosing big numbers
int promptAndGetNumLarge() {
    int numLarge = 0;

    // Display prompt and get the input
    cout << endl;
    cout << "Let's play Countdown!" << endl;
    cout << "How many big numbers would you like (0-4)? " << endl;
    cin >> numLarge;

    // Repeat if the input isn't valid (valid is between 0 and 4, inclusive)
    while (numLarge < 0 || numLarge > 4) {
        cout << "That's not a valid number. You must choose between 0 and 4." << endl;
        cout << "How many large numbers would you like? ";
        cin >> numLarge;
    }

    // Return the number chosen
    return numLarge;
}

// Determines the pieces of a calculation from the user input
void processCalculation(string userInput, int& num1, char& operation, int& num2) {
    // Use a stringstream (essentially turning the given string into a stream)
    stringstream ss(userInput);

    // Input the first number, then the operation, and then the second number from that stream
    ss >> num1 >> operation >> num2;
}
