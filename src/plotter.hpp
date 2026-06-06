#ifndef GRAPHCALC_PLOTTER_HPP
#define GRAPHCALC_PLOTTER_HPP

#include <vector>
#include <string>
#include <utility>

class Plotter {
public:
    Plotter(int width, int height);

    void clear();
    void setDot(int x, int y);
    void setDot(int x, int y, int color); // Set dot with a specific color
    void drawLine(int x0, int y0, int x1, int y1, int color = 0);
    void drawAxes(double xMin, double xMax, double yMin, double yMax);
    std::string render();

private:
    int width;   // Terminal characters width
    int height;  // Terminal characters height
    int gridW;   // Dot width (width * 2)
    int gridH;   // Dot height (height * 4)
    std::vector<unsigned char> canvas; // Stores the Braille dots
    std::vector<int> colorMap;         // Stores the color for each character cell

    // Map grid (x, y) to Braille dot bit
    unsigned int getBrailleChar(unsigned char dots);
};

#endif //GRAPHCALC_PLOTTER_HPP
