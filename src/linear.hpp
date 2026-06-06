// Created by msxshankar on 02/11/22.

#ifndef GRAPHCALC_LINEAR_HPP
#define GRAPHCALC_LINEAR_HPP

#include <vector>
#include <utility>

// Linked list used to store equation
struct equation {
    double coefficient;
    double power;
    equation *next;
};

// Inherits from graph class
class Linear {
private:
    equation *head = nullptr;
    double y;
    double x;
    double c;
    double yIntercept;
    double xIntercept;
    double gradient;
    double perpengradient;
    std::vector<std::pair<double, double>> coordinates;

public:
    Linear();

    ~Linear() {};

    double intercepts(double y, double m, double c);
    void generateCoordinates(double m, double c, double xMin, double xMax, double step);
    double perpendicular(double y, double m, double c);
    const std::vector<std::pair<double, double>>& getCoordinates() const { return coordinates; }
};

#endif //GRAPHCALC_LINEAR_HPP