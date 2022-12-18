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
    std::vector<double> coordinates;

public:
    Linear() {};

    double intercepts(double y, double m, double c);
    double coorindates(double y, double m, double c);
};