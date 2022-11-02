// Graphcalc by Mayur Shankar

// Include libraries and files
#include <iostream>
#include <regex>
#include <stdexcept>

#include "input.hpp"
#include "funcDeclarations.hpp"
#include "graph.hpp"

int main () {
    // Input class initialisation
    Input init;
    welcome(init);

	return 0;
}

// First screen displayed for user
int welcome(Input init) {
    std::cout << "Welcome to GraphCalc!" << std::endl;
    std::cout << "To begin with, please enter an equation for GraphCalc to work on" << std::endl;
    std::cout << "Supported equations are shown below:" << std::endl;
    std::cout << "1. Linear (please input in the form y=mx+c)" << std::endl;
    std::cout << "Enter equation> ";

    // Accepts user input and validates it
    std::cin >> init.linEquation;
    validation(string init.linEquation);
}

// Validation on inputted string
int validation (string equation) {
    // Matches equation and performs validation
    // Can be implemented in a separate method

    std::regex expr("y=mx\\+c");
    if (regex_match(init.linEquation, expr)) {
        userInput();

    }

    else {
        throw std::invalid_argument("Invalid input");
    }

    return 0;
}

int userInput () {
    // Ask for user input
    double xMin, yMin;
    std::cin >> xMin;
    std::cin >> yMin;

    // Create new Graph class
    Graph newGraph (xMin, yMin);
    return 0;
}