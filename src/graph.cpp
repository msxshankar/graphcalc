//
// Created by mayur on 23/10/22.
//
#include <iostream>

#include "graph.hpp"

// Constructor to accept minimum x and y values
//Graph::Graph(double xMin, double xMax, double increment) : xMin(xMin), xMax(xMax), increment(increment) {
//}

Graph::Graph(double xMinInp, double xMaxInp, double incrementInp) {
    xMin = xMinInp;
    xMax = xMaxInp;
    increment = incrementInp;
}
