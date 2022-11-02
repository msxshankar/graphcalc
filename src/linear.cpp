//
// Created by mayur on 02/11/22.
//

#include "linear.hpp"

double Linear::intercepts(double yIntercept, double xIntercept, double gradient) {
    yIntercept = yIntercept;
    xIntercept = (0-yIntercept) / gradient;

    return yIntercept, xIntercept;
}