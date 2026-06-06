//
// Created by mayur on 02/11/22.
//
#include <iostream>
#include <vector>

#include "linear.hpp"

Linear::Linear() {
    // head is now initialized to nullptr in header
}


// Below methods assume inputted equation is in the form y = mx + c

// Calculates intercepts
double Linear::intercepts(double y, double m, double c) {
    // Equation: y_in * Y = m * X + c
    // If y_in = 1, then Y = m * X + c
    
    yIntercept = c / y;
    if (m != 0) {
        xIntercept = -c / m;
    } else {
        xIntercept = 0; // Or handle as undefined
    }

    std::cout << "\nSTATISTICS:" << std::endl;
    std::cout << "Vertical (y) intercept: " << yIntercept << std::endl;
    std::cout << "Horizontal (x) intercept: " << (m != 0 ? std::to_string(xIntercept) : "None") << std::endl;

    return 0;
}

// Finds perpendicular line to inputted equation
double Linear::perpendicular(double y, double m, double c) {
    if (m != 0) {
        perpengradient = -1.0 / (m / y);
        std::cout << "Perpendicular equation: y = " << perpengradient << "x + " << (c / y) << " (through y-intercept)" << std::endl;
    } else {
        std::cout << "Perpendicular equation: x = 0 (vertical line)" << std::endl;
    }

    return 0;
}

// Generate coordinates for plotting
void Linear::generateCoordinates(double m, double c, double xMin, double xMax, double step) {
    coordinates.clear();
    for (double xVal = xMin; xVal <= xMax; xVal += step) {
        double yVal = m * xVal + c;
        coordinates.push_back({xVal, yVal});
    }
}
