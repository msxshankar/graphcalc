// Created by msxshankar on 12/11/22.

#ifndef GRAPHCALC_MENU_HPP

#define GRAPHCALC_MENU_HPP

#endif //GRAPHCALC_MENU_HPP

#include <iostream>

#include "menu.hpp"
#include "colour.hpp"
#include "linear.hpp"

// First screen displayed for user
void Menu::welcome() {
    std::cout << FGRED("Welcome to GraphCalc!") << std::endl;
    std::cout << "To begin with, please enter an equation for GraphCalc to work on" << std::endl;
    std::cout << "Supported equations are shown below:" << std::endl;
    std::cout << "1. Linear (please input in the form y=mx+c)" << std::endl;
    std::cout << "2. Polynomial (please input in the form y=x^2+c)" << std::endl;
}

int Menu::choice() {
    std::cout << "Please choose an option (1-2) > ";
    std::cin >> choiceNumber;

    // validate input
    while (std::cin.fail() || choiceNumber > 2 || choiceNumber < 1) {
        std::cout << "Please enter a valid choice (1-2) > ";
        //std::cin.clear();
        //std::cin.ignore(256,'\n');
        std::cin >> choiceNumber;
    }
    return SUCCESS;
}

int Menu::showGraph () {

    // Determine which type of graph to calculate
    switch (choiceNumber) {
        case 1: {

            // Calls Linear class methods
            std::unique_ptr<Linear>linObject(new Linear());
            linObject->input();
            linObject->validate(choiceNumber);
            linObject->intercepts();
            linObject->perpendicular();
            linObject->coorindates();
            //linObject->draw();
            break;
        }

        case 2: {
            std::cout << "Exponentials will be implemented at a later date!" << std::endl;
            break;
        }

        default: {
            std::cout << "Number out of range!" << std::endl;
            break;
        }
    }
    return 0;
}