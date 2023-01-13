// Created by msxshankar on 02/11/22.

#ifndef GRAPHCALC_LINEAR_HPP
#define GRAPHCALC_LINEAR_HPP

#endif //GRAPHCALC_LINEAR_HPP

// Linked list used to store equation
struct equation {
    double coefficient;
    double power;
    equation *next;
};

// Inherits from graph class
class Linear {
private:
    equation *head;
    double y;
    double x;
    double c;
    double yIntercept;
    double xIntercept;
    double gradient;
    double perpengradient;
    std::vector<double> coordinates;

public:
    Linear();

    ~Linear() {};

    double intercepts(double y, double m, double c);
    double coorindates(double y, double m, double c);
    double perpendicular(double y, double m, double c);
};