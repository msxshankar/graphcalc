#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "menu.hpp"
#include "linear.hpp"
#include "plotter.hpp"
#include "graph3d.hpp"

// First screen displayed for user
void Menu::welcome() {
    std::cout << "\033[1;34m" << "========================================" << "\033[0m" << std::endl;
    std::cout << "\033[1;36m" << "        WELCOME TO GRAPHCALC!           " << "\033[0m" << std::endl;
    std::cout << "\033[1;34m" << "========================================" << "\033[0m" << std::endl;
    std::cout << "Supported equations:" << std::endl;
    std::cout << "1. Linear (y = mx + c)" << std::endl;
    std::cout << "2. 3D Surface Plot (z = f(x, y))" << std::endl;
}

int Menu::choice() {
    std::cout << "\nPlease choose an option (1-2) > ";
    std::cin >> choiceNumber;

    while (std::cin.fail() || choiceNumber > 2 || choiceNumber < 1) {
        std::cout << "Please enter a valid choice (1-2) > ";
        std::cin.clear();
        std::cin.ignore(256,'\n');
        std::cin >> choiceNumber;
    }
    return choiceNumber;
}

template<typename T>
T Menu::getInput(const std::string& prompt, T defaultValue) {
    std::cout << prompt << " [" << defaultValue << "] > " << std::flush;
    std::string input;
    
    // Clear any leftover newline from previous cin >>
    if (std::cin.peek() == '\n') std::cin.ignore();
    
    std::getline(std::cin, input);
    if (input.empty()) return defaultValue;
    try {
        if constexpr (std::is_same_v<T, int>) return std::stoi(input);
        if constexpr (std::is_same_v<T, double>) return std::stod(input);
    } catch (...) {
        return defaultValue;
    }
    return defaultValue;
}

void Menu::interactiveLinear() {
    char input;
    double xMin = -10, xMax = 10, yMin = -10, yMax = 10;
    
    while (true) {
        std::cout << "\033[2J\033[H"; 
        Linear lin;
        lin.intercepts(y_in, m, c);
        
        Plotter plotter(60, 20);
        plotter.drawAxes(xMin, xMax, yMin, yMax);
        
        double gridW = 60 * 2;
        for (int i = 0; i < gridW; ++i) {
            double xVal = xMin + (i / (gridW - 1)) * (xMax - xMin);
            double yVal = (m * xVal + c) / y_in;
            if (yVal >= yMin && yVal <= yMax) {
                int py = static_cast<int>((yVal - yMin) / (yMax - yMin) * (80 - 1));
                plotter.setDot(i, py);
            }
        }
        
        std::cout << plotter.render();
        std::cout << "\nCURRENT EQUATION: " << y_in << "y = " << m << "x + " << c << std::endl;
        std::cout << "[m] Change Gradient | [c] Change Intercept | [y] Change Y-Coeff | [q] Quit" << std::endl;
        std::cout << "Selection > ";
        std::cin >> input;
        
        if (input == 'q') break;
        if (input == 'm') m = getInput("New m", m);
        if (input == 'c') c = getInput("New c", c);
        if (input == 'y') y_in = getInput("New y-coeff", y_in);
    }
}

void Menu::interactive3D() {
    Graph3D engine;
    double time = 0;
    int frame = 0;

    std::cout << "Starting 3D visualization... (Press Ctrl+C to stop, or wait for 100 frames)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (frame < 200) { // Limit for demonstration, can be infinite
        std::cout << "\033[2J\033[H";
        Plotter plotter(80, 30);
        engine.generateSurface(time);
        
        const auto& mesh = engine.getMesh();
        int res = mesh.size();
        
        for (int i = 0; i < res; ++i) {
            for (int j = 0; j < res; ++j) {
                Point2D p1 = engine.project(mesh[i][j], 160, 120);
                
                // Color based on Z height (map -3 to 3 to colors 20-230)
                int color = 20 + static_cast<int>((mesh[i][j].z + 3.0) / 6.0 * 210.0);
                
                if (i + 1 < res) {
                    Point2D p2 = engine.project(mesh[i+1][j], 160, 120);
                    plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                }
                if (j + 1 < res) {
                    Point2D p2 = engine.project(mesh[i][j+1], 160, 120);
                    plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                }
            }
        }
        
        std::cout << plotter.render();
        std::cout << "3D SURFACE: z = 3 * sin(sqrt(x^2 + y^2) - t) | Frame: " << frame << std::endl;
        
        time += 0.1;
        frame++;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int Menu::showGraph () {
    switch (choiceNumber) {
        case 1: {
            y_in = getInput("Enter y-coefficient", 1.0);
            m = getInput("Enter m value (gradient)", 1.0);
            c = getInput("Enter c value (y-intercept)", 0.0);
            interactiveLinear();
            break;
        }
        case 2: {
            interactive3D();
            break;
        }
    }
    return 0;
}