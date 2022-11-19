// Graphcalc by Mayur Shankar

// Include libraries and files
#include <iostream>
#include <stdexcept>

#include "menu.hpp"
//#include "funcDeclarations.hpp"
//#include "graph.hpp"
#include "linear.hpp"

int main (int argc, char **argv) {
    // Input class initialisation
    Menu init;

    // Call all methods
    init.welcome();
    init.validation();

	return 0;
}