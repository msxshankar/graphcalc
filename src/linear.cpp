//
// Created by mayur on 02/11/22.
//
#include <iostream>
#include <vector>

#include "linear.hpp"

Linear::Linear() {
    head->coefficient = 0;
}


// Below methods assume inputted equation is in the form y = mx + c

// Calculates intercepts
double Linear::intercepts(double y, double m, double c) {
    yIntercept = ((m*0) + c) / y;
    xIntercept = ((y*0) - c) / m;
    std::cout << "STATISTICS:" << std::endl;
    std::cout << "Vertical (y) intercept: " << yIntercept << std::endl;
    std::cout << "Horizontal (x) intercept: " << xIntercept << std::endl;

    return 0;
}

// Finds perpendicular line to inputted equation
double Linear::perpendicular(double y, double m, double c) {
    gradient = m;
    perpengradient = (1 / gradient) * -1;
    std::cout << "Perpendicular equation: y = " << perpengradient << "x +" << c << std::endl;

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

    std::cout << "COORDINATES:" << std::endl;
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
