// Created by msxshankar on 02/11/22.

#ifndef GRAPHCALC_LINEAR_HPP
#define GRAPHCALC_LINEAR_HPP

#endif //GRAPHCALC_LINEAR_HPP

#include <vector>

#include "graph.hpp"

// Inherits from graph class
class Linear : public Graph {
private:
    double y;
    double x;
    double c;
    double yIntercept;
    double xIntercept;
    double gradient;
    double perpengradient;
    std::vector<double> coordinates;

public:
    //Linear();

    ~Linear() {};

    double intercepts();
    double coorindates();
    double perpendicular();
};