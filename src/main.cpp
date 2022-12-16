// Graphcalc by Mayur Shankar

// Include libraries and files
#include <stdexcept>

#include "menu.hpp"
//#include "funcDeclarations.hpp"

int main (int argc, char **argv) {
    // Input class initialisation
    Menu init;

    // Call all methods
    init.welcome();
    init.choice();
    init.showGraph();
    //init.validation();

	return 0;
}