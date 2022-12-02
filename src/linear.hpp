//
// Created by mayur on 02/11/22.
//
#ifndef GRAPHCALC_LINEAR_HPP
#define GRAPHCALC_LINEAR_HPP

#endif //GRAPHCALC_LINEAR_HPP

#include "graph.hpp"

// Inherits from graph class
class Linear : public Graph {
private:
    double yIntercept;
    double xIntercept;
    double gradient;

public:
    Linear() {};

    double intercepts(double yIntercept, double xIntercept, double gradient);
    double coorindates();
};