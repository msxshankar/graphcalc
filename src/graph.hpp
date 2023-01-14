// Created by mxshankar on 23/10/22.

#ifndef GRAPHCALC_GRAPHS_HPP
#define GRAPHCALC_GRAPHS_HPP

#endif //GRAPHCALC_GRAPHS_HPP

#define SUCCESS 0
#define FAILURE 1

#include <memory>
#include <list>

// Node used to store coefficient and power
struct equation {
    double coefficient;
    double power;
};

// Class containing graph equations
class Graph {

protected:
    // Linked list to store equation
    std::list<struct equation> equationList;

    // Unique pointer to each node in linked list
    std::unique_ptr<struct equation> ptr;

    // Min and max range
    double min, max;

public:
   int stopCondition;

   Graph();

   int input();
   //int simplify();

};