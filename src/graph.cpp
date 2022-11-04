//
// Created by mayur on 23/10/22.
//
#include <iostream>

#include "graph.hpp"

// Constructor to accept minimum x and y values
Graph::Graph(double xMin, double yMin, double increment) : xMin(xMin), yMin(yMin), increment(increment) {
   std::cout << xMin << yMin << increment << std::endl;
}
