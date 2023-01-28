// Graphcalc by msxshankar (2023)

// Include libraries and files
#include <iostream>

#include "main.hpp"
#include "menu.hpp"

// namespace for global variables
namespace global {
    const std::string version = "0.9 beta";
}

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

// Accepts command line arguments
void argumentParser (char *argv[]) {
    const std::string argument = argv[1];

    if (argument == "-h" || argument == "--help") {
        std::cout << "usage: graphcalc [-h] [-v]"
                  << "optional arguments:" << std::endl
                  << "-h, --help        show help message" << std::endl
                  << "-v, --version     show version number" << std::endl
                  << "Program usage:" << std::endl
                  << "Run the program and enter equation. Statistics of graph will be displayed" << std::endl
                  << std::endl;
        exit(ARGUMENT);
    }

    else if (argument == "-v" || argument == "--version") {
        std::cout << "graphcalc version " << global::version << std::endl;
        exit(VERSION);
    }

    else {
        std::cout << "Unknown argument: " << argument << std::endl;
        std::cout << "Use -h or --help for help" << std::endl;
        exit(FAIL);
    }
}