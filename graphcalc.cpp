// Graphcalc by Mayur Shankar

// Include libraries and files
#include <iostream>
#include "graphcalc.h"
#include "funcDeclarations.h"

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

    return 0;
}
