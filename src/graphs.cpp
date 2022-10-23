//
// Created by mayur on 23/10/22.
//
#include <iostream>

#include "graphs.hpp"

// Constructor to accept minimum x and y values
Graph::Graph(double xMin, double yMin) {
   std::cout << "Enter range of values" << std::endl << "> ";
   std::cin >> xMin;
   std::cin >> yMin;
}
