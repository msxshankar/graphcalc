#ifndef GRAPHCALC_GRAPH3D_HPP
#define GRAPHCALC_GRAPH3D_HPP

#include <vector>

struct Point3D {
    double x, y, z;
};

struct Point2D {
    int x, y;
};

class Graph3D {
public:
    Graph3D();
    
    // Generates a surface mesh for z = sin(sqrt(x^2 + y^2))
    void generateSurface(double time);
    
    // Projects 3D points to 2D grid coordinates
    Point2D project(const Point3D& p, int gridW, int gridH);
    
    const std::vector<std::vector<Point3D>>& getMesh() const { return mesh; }

private:
    std::vector<std::vector<Point3D>> mesh;
    double angleX = 0.5;
    double angleY = 0.5;
    double angleZ = 0.0;
};

#endif //GRAPHCALC_GRAPH3D_HPP
