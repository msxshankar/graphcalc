// Graphcalc by Mayur Shankar

// Include libraries and files
#include <iostream>
#include <regex>
#include <stdexcept>

#include "input.hpp"
#include "funcDeclarations.hpp"
#include "graph.hpp"
//#include "linear.hpp"

int main (int argc, char **argv) {
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
    validation(init);
}

// Validation on inputted string
int validation (Input init) {
    // Matches equation and performs validation
    // Can be implemented in a separate method

    std::regex expr("y=[0-9]x\\+[0-9]");
    //std::regex expr(/^[+-]?((\d+(\.\d*)?)|(\.\d+) ) $/x)
    if (regex_match(init.linEquation, expr)) {
        init.values();
        userInput();
    }

    else {
        throw std::invalid_argument("Invalid input");
    }

    return 0;
}

int userInput () {
    // Ask for user input
    double xMin, xMax, yMin, increment;
    std::cout << "Enter range of x values to be shown (min max) > ";
    std::cin >> xMin;
    std::cin >> xMax;
    std::cout << "Enter increment > ";
    std::cin >> increment;

    // Create new Graph class
    Graph newGraph (xMin, yMin, increment);
    //Linear one;
    //one.intercepts();
    return 0;
}