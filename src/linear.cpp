//
// Created by mayur on 02/11/22.
//
#include <iostream>
#include <vector>

#include "linear.hpp"

double Linear::intercepts(double yIntercept, double xIntercept, double gradient) {
    yIntercept = yIntercept;
    xIntercept = (0-yIntercept) / gradient;

    return yIntercept, xIntercept;
}

// Vector of coorindates
double Linear::coorindates() {
    Input inp1;

    int minX = -10;
    int maxX = 10;

    std::vector<double> coordinates;

    for (int i = minX ; i < maxX ; i++ ) {
       coordinates.push_back(i);
    }

    for (auto j: coordinates) {
        std::cout << j << ' ';
    }

    return 0;
}