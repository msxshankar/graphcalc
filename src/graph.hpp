// Created by mxshankar on 23/10/22.

#ifndef GRAPHCALC_GRAPHS_HPP
#define GRAPHCALC_GRAPHS_HPP

#endif //GRAPHCALC_GRAPHS_HPP

#define SUCCESS 0
#define FAILURE 1

#include <memory>
#include <list>

// Linked list used to store equation
struct equation {
    double coefficient;
    double power;
};

// Class containing graph equations
class Graph {

protected:
    // Linked list to store equation
    std::list<equation> equationList;
   //std::shared_ptr<struct equation> head;

public:
   int stop;

   Graph();

   int input();
   //~Graph();
};