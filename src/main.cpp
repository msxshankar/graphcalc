// Graphcalc by msxshankar (2023)

// Include libraries and files
#include <iostream>
#include <stdexcept>

#include "main.hpp"
#include "menu.hpp"
//#include "funcDeclarations.hpp"

int main (int argc, char *argv[]) {

    // Parse arguments
    if (argc > 1) {
        argumentParser(argv);
    }
    // Input class initialisation
    Menu menu;

    // Call all methods
    menu.welcome();
    menu.choice();
    menu.showGraph();
    //init.validation();

	return 0;
}

void argumentParser (char *argv[]) {
    std::cout << argv[1];

    if (argv[1] == "-h" || argv[1] == "--help") {
        std::cout << "usage: graphcalc [-h]"
                  << "optional arguments:"
                  << "-h, --help        show help message"
                  << std::endl;
    }
}