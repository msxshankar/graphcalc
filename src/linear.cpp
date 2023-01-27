//
// Created by mayur on 02/11/22.
//
#include <iostream>
#include <vector>
#include <cmath>

#include "linear.hpp"
#include "matplot/matplot.h"

// Below methods assume inputted equation is in the form y = mx + c

// Calculates intercepts
double Linear::intercepts() {
    auto iter = equationList.begin();

    yIntercept = ((iter++->coefficient*0) + iter->coefficient);
    xIntercept = (0 - iter-- ->coefficient) / iter->coefficient;
    std::cout << "STATISTICS:" << std::endl;
    std::cout << "Vertical (y) intercept: " << yIntercept << std::endl;
    std::cout << "Horizontal (x) intercept: " << xIntercept << std::endl;

    return 0;
}

// Finds perpendicular line to inputted equation
double Linear::perpendicular() {
    auto iter = equationList.begin();

    gradient = iter->coefficient;
    perpengradient = (1 / gradient) * -1;
    std::cout << "Perpendicular equation: y = " << perpengradient << "x +" << c << std::endl;

    return 0;
}

// Vector of coorindates
double Linear::coorindates() {

    auto iter = equationList.begin();

    // Add points to vector
    // Be careful with typecasting
    for (int i = (int) min; i < (int) max ; i++ ) {
       coordinates.push_back(i);
       coordinates.push_back((iter ->coefficient*i) + iter->coefficient);
    }

    std::cout << "COORDINATES:" << std::endl;
    for (int j = 0; j < coordinates.size(); j++) {
        if (j % 2 == 0) {
            std::cout << coordinates[j] << ',';
        }
        else {
            std::cout << coordinates[j] << ' ';
        }
    }
    return 0;
}

int Linear::draw() {
   //matplot::plot(coordinates, coordinates);
   //matplot::show();

    std::vector<double> Y = {16, 5, 9, 4};

    matplot::plot(Y);

    matplot::show();
    return 0;
}