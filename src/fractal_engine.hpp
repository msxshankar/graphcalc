#ifndef GRAPHCALC_FRACTAL_ENGINE_HPP
#define GRAPHCALC_FRACTAL_ENGINE_HPP

#include <vector>
#include "graph3d.hpp" // for Point3D

namespace FractalEngine {

// Evaluates Mandelbrot iteration count for complex point (cr, ci)
int evalMandelbrot(double cr, double ci, int maxIter = 30);

// Evaluates Julia iteration count for complex point (zr, zi) with constant (cr, ci)
int evalJulia(double zr, double zi, double cr, double ci, int maxIter = 30);

// Evaluates Plasma value at (px, py) at given time
double evalPlasma(double px, double py, double time, double a = 1.0, double b = 1.0, double c = 0.0);

// Checks if point (px, py) is inside Sierpinski Carpet
bool inSierpinski(double px, double py, int maxDepth = 4);

// Generates Clifford attractor points into output vector
void generateClifford(int numPoints, double a, double b, double c, double d, std::vector<Point3D>& outPoints);

// Generates Barnsley fern points into output vector
void generateBarnsley(int numPoints, double paramA, double paramB, double paramC, std::vector<Point3D>& outPoints);

// Generates Lorenz attractor trajectory points into output vector
void generateLorenz(int numPoints, double paramA, double paramB, std::vector<Point3D>& outPoints);

// Generates Aizawa attractor trajectory points into output vector
void generateAizawa(int numPoints, double paramA, double paramB, double paramC, std::vector<Point3D>& outPoints);

} // namespace FractalEngine

#endif // GRAPHCALC_FRACTAL_ENGINE_HPP
