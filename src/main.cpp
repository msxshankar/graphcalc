// Graphcalc by Mayur Shankar

// Include libraries and files
#include <stdexcept>

#include "menu.hpp"
//#include "funcDeclarations.hpp"

int main (int argc, char **argv) {
    // Input class initialisation
    Menu menu;

    // Call all methods
    menu.welcome();
    menu.choice();
    menu.showGraph();
    //init.validation();

	return 0;
}