//
// Created by mayur on 23/10/22.
//
#include <iostream>

#include "graphs.hpp"

// Constructor to accept minimum x and y values
Graph::Graph(double xMin, double yMin) : xMin(xMin), yMin(yMin) {
   std::cout << xMin << yMin << std::endl;
}
