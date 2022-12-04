//
// Created by mayur on 12/11/22.
//

#include <iostream>
#include <string>
#include <regex>

#include "menu.hpp"
#include "linear.hpp"

// First screen displayed for user
int Menu::welcome() {
    std::cout << "Welcome to GraphCalc!" << std::endl;
    std::cout << "To begin with, please enter an equation for GraphCalc to work on" << std::endl;
    std::cout << "Supported equations are shown below:" << std::endl;
    std::cout << "1. Linear (please input in the form y=mx+c)" << std::endl;
    std::cout << "Enter equation> ";

    // Accepts user input and validates it
    std::cin >> linEquation;

    return 0;
}

// Validation on inputted string
int Menu::validation () {
    // Matches equation and performs validation
    // Can be implemented in a separate method

    std::regex expr("y=[0-9]x\\+[0-9]");
    //std::regex expr(/^[+-]?((\d+(\.\d*)?)|(\.\d+) ) $/x)

    if (regex_match(linEquation, expr)) {
        //init.values();
        //userInput();
    }

    else {
        throw std::invalid_argument("Invalid input");
    }

    return 0;
}
int Menu::linearUserInput () {

    // Ask for user input
    std::cout << "Please enter in the form: y = mx + c" << std::endl;
    std::cout << "Enter y value > ";
    std::cin >> y;
    std::cout << "Enter m value > ";
    std::cin >> m;
    std::cout << "Enter c value > ";
    std::cin >> c;

    //std::cout << "Enter range of x values to be shown (min, max) > ";
    //std::cout << "Enter increment > ";

    // Create new Graph class
    /*
    Graph newGraph (xMin, xMax, increment);
    std::cout << newGraph.xMin;
    */

    // Calls Linear class methods
    Linear lin1;
    lin1.intercepts(y, m, c);
    return 0;
}

int Menu::values() {
    int index = linEquation.find("=");

    std::cout << index;

    return index;
}