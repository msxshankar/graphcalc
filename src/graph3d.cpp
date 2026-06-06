#include "graph3d.hpp"
#include <cmath>

Graph3D::Graph3D() {
    // Initial mesh setup
}

void Graph3D::generateSurface(double time) {
    mesh.clear();
    int res = 20;
    double range = 10.0;
    
    for (int i = 0; i < res; ++i) {
        std::vector<Point3D> row;
        double x = -range + (i * 2.0 * range / (res - 1));
        for (int j = 0; j < res; ++j) {
            double y = -range + (j * 2.0 * range / (res - 1));
            
            // Ripple function: z = sin(sqrt(x^2 + y^2) - time)
            double r = std::sqrt(x*x + y*y);
            double z = 3.0 * std::sin(r - time);
            
            row.push_back({x, y, z});
        }
        mesh.push_back(row);
    }
    
    // Increment angles for dynamic movement
    angleY += 0.02;
    angleX = 0.6 + 0.2 * std::sin(time * 0.5);
}

Point2D Graph3D::project(const Point3D& p, int gridW, int gridH) {
    // 1. Rotation
    double x = p.x;
    double y = p.y;
    double z = p.z;
    
    // Rotate Y
    double tx = x * std::cos(angleY) + z * std::sin(angleY);
    double tz = -x * std::sin(angleY) + z * std::cos(angleY);
    x = tx; z = tz;
    
    // Rotate X
    double ty = y * std::cos(angleX) - z * std::sin(angleX);
    tz = y * std::sin(angleX) + z * std::cos(angleX);
    y = ty; z = tz;
    
    // 2. Perspective Projection
    double fov = 300.0;
    double distance = 20.0;
    double factor = fov / (distance + z);
    
    int px = static_cast<int>(x * factor + gridW / 2.0);
    int py = static_cast<int>(y * factor + gridH / 2.0);
    
    return {px, py};
}
