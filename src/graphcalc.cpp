// Graphcalc by Mayur Shankar

// Include libraries and files
#include <iostream>
#include <regex>
#include <stdexcept>

#include "graphcalc.hpp"
#include "funcDeclarations.hpp"

int main () {
    // Basic class initialisation
    Calc f;

    welcome(f);
	return 0;
}

// First screen displayed for user
int welcome(Calc init) {
    std::cout << "Welcome to GraphCalc!" << std::endl;
    std::cout << "To begin with, please enter an equation for GraphCalc to work on" << std::endl;
    std::cout << "Supported equations are shown below:" << std::endl;
    std::cout << "-> y=mx+c" << std::endl;
    std::cout << "> ";

    std::cin >> init.linEquation;
    std::cout << init.linEquation;

    // Matches equation and performs validation
    // Can be implemented in a separate method
    std::regex expr("y=mx\\+c");
    if (regex_match(init.linEquation, expr)) {
        std::cout << "Successful input" << std::endl;
    }

    else {
        throw std::invalid_argument("Invalid input");
    }

    return 0;
}
