// Graphcalc by msxshankar (2023)

// Include libraries and files
#include <iostream>
#include <clocale>

#include "main.hpp"
#include "tui.hpp"

// namespace for global variables
namespace global {
    const std::string version = "1.0 TUI Edition";
}

int main (int argc, char *argv[]) {
    std::setlocale(LC_ALL, "");

    // Parse arguments
    if (argc > 1) {
        argumentParser(argv);
    }
    
    // TUI initialization and run
    TUI tui;
    return tui.run();
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
}