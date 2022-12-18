//
// Created by mayur on 02/11/22.
//
#include <iostream>
#include <vector>

#include "linear.hpp"

double Linear::intercepts(double y, double m, double c) {
    yIntercept = ((m*0) + c) / y;
    xIntercept = ((y*0) - c) / m;
    std::cout << "Vertical (y) intercept: " << yIntercept << std::endl;
    std::cout << "Horizontal (x) intercept: " << xIntercept << std::endl;

    return 0;
}

// Vector of coorindates
double Linear::coorindates(double y, double m, double c) {

    int minX = 0;
    int maxX = 10;

    // Add points to vector
    for (int i = minX ; i < maxX ; i++ ) {
       if (i % 2 == 1) {
           coordinates.push_back((m*i)+c);
       }
       else {
           coordinates.push_back(i);
       }
    }

    for (int j: coordinates) {
        if (j % 2 == 0) {
            std::cout << j << ',';
        }
        else {
            std::cout << j << ' ';
        }
    }
    return 0;
}