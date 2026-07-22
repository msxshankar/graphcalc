#include "fractal_engine.hpp"
#include <cmath>
#include <cstdint>

namespace FractalEngine {

int evalMandelbrot(double cr, double ci, int maxIter) {
    double zr = 0.0;
    double zi = 0.0;
    int iter = 0;
    double zr2 = 0.0;
    double zi2 = 0.0;
    while (zr2 + zi2 <= 4.0 && iter < maxIter) {
        zi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
        zr2 = zr * zr;
        zi2 = zi * zi;
        iter++;
    }
    return iter;
}

int evalJulia(double zr, double zi, double cr, double ci, int maxIter) {
    int iter = 0;
    double zr2 = zr * zr;
    double zi2 = zi * zi;
    while (zr2 + zi2 <= 4.0 && iter < maxIter) {
        zi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
        zr2 = zr * zr;
        zi2 = zi * zi;
        iter++;
    }
    return iter;
}

double evalPlasma(double px, double py, double time, double a, double b, double c) {
    double val = std::sin(px * a + time) +
                 std::cos(py * b - time) +
                 std::sin((px + py) * c + time * 1.5) +
                 std::cos(std::sqrt(px * px + py * py) * 2.0 - time * 2.0);
    double norm = (val + 4.0) / 8.0;
    if (norm < 0.0) norm = 0.0;
    if (norm > 1.0) norm = 1.0;
    return norm;
}

bool inSierpinski(double px, double py, int maxDepth) {
    double tx = (px + 2.0) / 2.7;
    double ty = (py + 1.2) / 2.4;
    if (tx < 0.0 || tx > 1.0 || ty < 0.0 || ty > 1.0) return false;
    
    for (int step = 0; step < maxDepth; ++step) {
        int ix = static_cast<int>(tx * 3.0);
        int iy = static_cast<int>(ty * 3.0);
        if (ix == 1 && iy == 1) {
            return false;
        }
        tx = tx * 3.0 - ix;
        ty = ty * 3.0 - iy;
    }
    return true;
}

void generateClifford(int numPoints, double a, double b, double c, double d, std::vector<Point3D>& outPoints) {
    outPoints.clear();
    outPoints.reserve(numPoints);
    double cx = 0.1, cy = 0.1;
    for (int i = 0; i < numPoints; ++i) {
        double nextX = std::sin(a * cy) + c * std::cos(a * cx);
        double nextY = std::sin(b * cx) + d * std::cos(b * cy);
        cx = nextX;
        cy = nextY;
        outPoints.push_back({cx, cy, 0.0});
    }
}

void generateBarnsley(int numPoints, double paramA, double paramB, double paramC, std::vector<Point3D>& outPoints) {
    outPoints.clear();
    outPoints.reserve(numPoints);
    double bx = 0.0, by = 0.0;
    uint32_t rng = 12345;
    auto next_rand = [&rng]() {
        rng = rng * 1664525 + 1013904223;
        return static_cast<double>(rng) / 4294967296.0;
    };

    for (int i = 0; i < numPoints; ++i) {
        double r = next_rand();
        double nextX, nextY;
        if (r < 0.01) {
            nextX = 0.0;
            nextY = 0.16 * by;
        } else if (r < 0.86) {
            nextX = 0.85 * bx + 0.04 * by;
            nextY = -0.04 * bx + 0.85 * by + 1.6;
        } else if (r < 0.93) {
            nextX = 0.20 * bx - 0.26 * by;
            nextY = 0.23 * bx + 0.22 * by + 1.6;
        } else {
            nextX = -0.15 * bx + 0.28 * by;
            nextY = 0.26 * bx + 0.24 * by + 0.44;
        }
        double tx = nextX;
        double ty = nextY;
        tx += 0.05 * (paramA - 1.0) * ty * ty;
        ty *= paramB;
        tx += paramC * ty;

        bx = nextX;
        by = nextY;
        outPoints.push_back({tx, ty, 0.0});
    }
}

void generateLorenz(int numPoints, double paramA, double paramB, std::vector<Point3D>& outPoints) {
    outPoints.clear();
    outPoints.reserve(numPoints);
    double lx = 0.1, ly = 0.0, lz = 0.0;
    double sigma = 10.0;
    double rho = 28.0 * paramB;
    double beta = 8.0 / 3.0;
    double dt = 0.015 * paramA;
    if (dt < 0.001) dt = 0.001;

    for (int i = 0; i < 50; ++i) {
        double dx = sigma * (ly - lx);
        double dy = lx * (rho - lz) - ly;
        double dz = lx * ly - beta * lz;
        lx += dx * dt;
        ly += dy * dt;
        lz += dz * dt;
    }

    for (int i = 0; i < numPoints; ++i) {
        double dx = sigma * (ly - lx);
        double dy = lx * (rho - lz) - ly;
        double dz = lx * ly - beta * lz;
        lx += dx * dt;
        ly += dy * dt;
        lz += dz * dt;
        outPoints.push_back({lx, ly, lz - rho});
    }
}

void generateAizawa(int numPoints, double paramA, double paramB, double paramC, std::vector<Point3D>& outPoints) {
    outPoints.clear();
    outPoints.reserve(numPoints);
    double ax = 0.1, ay = 0.0, az = 0.0;
    double a = 0.95;
    double b = 0.7 * paramB;
    double c = 0.6;
    double d = 3.5 + paramC;
    double e = 0.25;
    double f = 0.1;
    double dt = 0.01 * paramA;
    if (dt < 0.001) dt = 0.001;

    for (int i = 0; i < 100; ++i) {
        double dx = (az - b) * ax - d * ay;
        double dy = d * ax + (az - b) * ay;
        double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
        ax += dx * dt;
        ay += dy * dt;
        az += dz * dt;
    }

    for (int i = 0; i < numPoints; ++i) {
        double dx = (az - b) * ax - d * ay;
        double dy = d * ax + (az - b) * ay;
        double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
        ax += dx * dt;
        ay += dy * dt;
        az += dz * dt;
        outPoints.push_back({ax * 1.5, ay * 1.5, (az - 1.2) * 1.5});
    }
}

} // namespace FractalEngine
