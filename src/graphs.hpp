//
// Created by mayur on 23/10/22.
//

#ifndef GRAPHCALC_GRAPHS_HPP
#define GRAPHCALC_GRAPHS_HPP

#endif //GRAPHCALC_GRAPHS_HPP

// Class containing all graphs
class Graph {

public:
   // Key graph attributes
   // Will be inherited by other types of graphs
   double xMin;
   double yMin;
   double xCross;
   double yCross;

   Graph();

   Graph(double xMin, double yMin);

   //~Graph();
};