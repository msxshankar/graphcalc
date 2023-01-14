//
// Created by mayur on 23/10/22.
//
#include <iostream>

#include "graph.hpp"

// Constructor to accept minimum x and y values
//Graph::Graph(double xMin, double xMax, double increment) : xMin(xMin), xMax(xMax), increment(increment) {
//}
/*
Graph::Graph(double xMinInp, double xMaxInp, double incrementInp) {
    xMin = xMinInp;
    xMax = xMaxInp;
    increment = incrementInp;
}
*/

// Constructor
Graph::Graph() : stop(0) {
   //head = std::make_shared<struct equation>();
}

int Graph::input() {

    std::cout << "Please enter equation. Start with largest powers of x" << std::endl;
    while (true) {
        std::cout << "Enter power of x: " << std::endl;
        equationList.push_back(equation());
        equationList++;

        std::cin >> equationList[0].power = 2;
        std::cout << "Enter coefficient: " << std::endl;
        //std::cin >> equationList.push_back(struct equation);
        std::cout << "Enter 1 to continue, 0 to stop" << std::endl;
        std::cin >> stop;
        if (stop == 0) {
            break;
        }
        else {
           //head->next = std::make_shared<struct equation>();
           //head = head->next;
        }
    }

    return SUCCESS;
}