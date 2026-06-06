#include "plotter.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

Plotter::Plotter(int w, int h) : width(w), height(h) {
    gridW = width * 2;
    gridH = height * 4;
    canvas.resize(width * height, 0);
    colorMap.resize(width * height, 0);
}

void Plotter::clear() {
    std::fill(canvas.begin(), canvas.end(), 0);
    std::fill(colorMap.begin(), colorMap.end(), 0);
}

void Plotter::setDot(int x, int y) {
    setDot(x, y, 0);
}

void Plotter::setDot(int x, int y, int color) {
    if (x < 0 || x >= gridW || y < 0 || y >= gridH) return;

    int cellX = x / 2;
    int cellY = (gridH - 1 - y) / 4;
    int dotX = x % 2;
    int dotY = (gridH - 1 - y) % 4;

    static const unsigned char dotMap[2][4] = {
        {0x01, 0x02, 0x04, 0x40},
        {0x08, 0x10, 0x20, 0x80}
    };

    int idx = cellY * width + cellX;
    canvas[idx] |= dotMap[dotX][dotY];
    if (color != 0) colorMap[idx] = color;
}

void Plotter::drawLine(int x0, int y0, int x1, int y1, int color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setDot(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

void Plotter::drawAxes(double xMin, double xMax, double yMin, double yMax) {
    if (yMin <= 0 && yMax >= 0) {
        int y0 = static_cast<int>((0 - yMin) / (yMax - yMin) * (gridH - 1));
        drawLine(0, y0, gridW - 1, y0, 244); // Gray
    }
    if (xMin <= 0 && xMax >= 0) {
        int x0 = static_cast<int>((0 - xMin) / (xMax - xMin) * (gridW - 1));
        drawLine(x0, 0, x0, gridH - 1, 244); // Gray
    }
}

std::string Plotter::render() {
    std::stringstream ss;
    int lastColor = -1;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            unsigned char dots = canvas[idx];
            int color = colorMap[idx];

            if (dots == 0) {
                if (lastColor != -1) { ss << "\033[0m"; lastColor = -1; }
                ss << " ";
            } else {
                if (color != lastColor) {
                    if (color == 0) ss << "\033[36m"; // Default Cyan
                    else ss << "\033[38;5;" << color << "m";
                    lastColor = color;
                }
                unsigned int code = 0x2800 + dots;
                ss << (char)(0xE2);
                ss << (char)(0xA0 + ((code >> 6) & 0x03));
                ss << (char)(0x80 + (code & 0x3F));
            }
        }
        ss << "\n";
    }
    ss << "\033[0m";
    return ss.str();
}
