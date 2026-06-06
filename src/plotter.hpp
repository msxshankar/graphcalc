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
    std::pair<std::string, std::string> getCell(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return {" ", ""};
        int idx = y * width + x;
        unsigned char dots = canvas[idx];
        int color = colorMap[idx];
        if (dots == 0) return {" ", ""};

        unsigned int code = 0x2800 + dots;
        std::string ch = "";
        ch += (char)(0xE2);
        ch += (char)(0xA0 + ((code >> 6) & 0x03));
        ch += (char)(0x80 + (code & 0x3F));

        std::string colStr = "";
        if (color == 0) colStr = "\033[36m"; // Default Cyan
        else colStr = "\033[38;5;" + std::to_string(color) + "m";

        return {ch, colStr};
    }


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
