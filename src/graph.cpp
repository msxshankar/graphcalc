//
// Created by mayur on 23/10/22.
//
#include <iostream>

#include "graph.hpp"

// Constructor
Graph::Graph() : stopCondition(0), ptr(std::make_unique<struct equation>()) {}

// Adding stdin values to linked list
int Graph::input() {

    std::cout << "Please enter equation. Start with largest powers of x" << std::endl;
    std::cout << "For example: y = ax^2 + bx + c" << std::endl;

    while (true) {
        std::cout << "Enter coefficient: ";
        std::cin >> ptr->coefficient;

        std::cout << "Enter power of x: ";
        std::cin >> ptr->power;

        equationList.push_back(*ptr);

        std::cout << "Enter 1 to continue, 0 to stop: ";
        std::cin >> stopCondition;
        if (stopCondition == 0) {
            break;
        }
    }

    // Output inputted equation
    auto iter = equationList.begin();
    int count = 1;

    std::cout << "Inputted equation: ";
    while (iter != equationList.end()) {
        std::cout << iter->coefficient << "x^" << iter->power << ((count == equationList.size()) ? "\n" : " + ");
        iter++, count++;
    }

    // Minimum and maximum range
    std::cout << "Next, enter minimum and maximum values of x (min max): ";
    std::cin >> min >> max;

    return SUCCESS;
}

// Simplifies inputted equation
/*
int Graph::simplify() {

    auto iter = equationList.begin();

    while (iter != equationList.end()) {

    }
    return SUCCESS;
}
*/