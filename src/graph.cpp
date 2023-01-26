//
// Created by mayur on 23/10/22.
//
#include <iostream>

#include "graph.hpp"

// Constructor
Graph::Graph() : ptr(std::make_unique<struct equation>()) {}

// Adding stdin values to linked list
int Graph::input() {

    std::cout << "Please enter coefficients and powers of equation. Start with largest powers of x" << std::endl;
    std::cout << "For example: y = ax^2 + bx + c" << std::endl;
    std::cout << "If you've finished inputting, type done" << std::endl;

    while (true) {
        std::cout << "Enter coefficient: ";
        std::cin >> entry;

        if (entry == "done" || entry == "DONE") {
           break;
        }
        else {
           try {
               ptr->coefficient = std::stod(entry);
           }

           // Invalid inputs
           catch (std::invalid_argument&) {
              std::cout << "Invalid input, please try again" << std::endl;
              std::cin.clear();
              continue;
           }
        }

        std::cout << "Enter power of x: ";
        std::cin >> entry;

        if (entry == "done" || entry == "DONE") {
           std::cout << "Power will be zero" << std::endl;
           break;
        }
        else {
            try {
                ptr->power = std::stod(entry);
            }

            catch (std::invalid_argument&) {
                std::cout << "Invalid input. Please try again" << std::endl;
                std::cin.clear();
                continue;
            }
        }
        // Add to linked list
        equationList.push_back(*ptr);

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
int Graph::simplify() {

    auto iter = equationList.begin();

    // Order list
    Graph::quicksort();

    while (iter != equationList.end()) {
        // Compare powers of adjacent terms
        if (iter++->power == iter->power) {
            // Add previous coefficient
            iter-- ->coefficient += iter->coefficient;
        }
    }
    return SUCCESS;
}

// Quick sort to sort list in ascending order
std::list<struct equation> quicksort () {

}