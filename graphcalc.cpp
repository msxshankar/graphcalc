// Graphcalc by Mayur Shankar

// Include libraries and files
#include <iostream>
#include <regex>
#include <stdexcept>

#include "graphcalc.hpp"
#include "funcDeclarations.hpp"

int main () {
    Calc f;

    welcome(f);
	return 0;
}

int welcome(Calc init) {
    std::cout << "Welcome to GraphCalc!" << std::endl;
    std::cout << "To begin with, please enter a linear equation in the form y=mx+c" << std::endl << "> ";
    std::cin >> init.linEquation;
    std::cout << init.linEquation;

    std::regex expr("y=mx+c");
    if (regex_match(init.linEquation, expr)) {
        std::cout << "Successful input" << std::endl;
    }
    else {
        throw std::invalid_argument("Invalid input");
    }

    return 0;
}
