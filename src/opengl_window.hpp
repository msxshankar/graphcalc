#ifndef GRAPHCALC_OPENGL_WINDOW_HPP
#define GRAPHCALC_OPENGL_WINDOW_HPP

#include <string>
#include "parser.hpp"

class OpenGLWindow {
public:
    OpenGLWindow();
    ~OpenGLWindow();

    // Opens a window to visualize the equation. Returns true on success.
    bool openWindow(const std::string& equationStr, bool is3D);

private:
    bool initX11GLX();
    void cleanup();
    void runEventLoop();
    void renderFrame();

    // Rendering methods
    void render2D();
    void render3D();
    void drawHUD();

    // Math parser
    MathParser parser;
    std::string equation;
    bool is3DMode;

    // Parameters that can be adjusted in real-time
    double paramA;
    double paramB;
    double paramC;
    double timeVal;
    bool isAnimated;

    // Viewport parameters (2D)
    double xMin, xMax;
    double yMin, yMax;

    // Viewport parameters (3D)
    double rotateX; // Rotation around X axis (pitch)
    double rotateY; // Rotation around Y axis (yaw)
    double zoomDistance;

    // Mouse tracking state
    bool mousePressedLeft;
    bool mousePressedRight;
    int lastMouseX;
    int lastMouseY;

    // X11 & GLX handles
    void* displayPtr;   // Display*
    unsigned long windowId; // Window
    void* glContextPtr; // GLXContext
    unsigned long deleteMessageAtom; // Atom for window close button
    unsigned long fontId; // Font X11 handle
    int windowWidth;
    int windowHeight;
};

#endif // GRAPHCALC_OPENGL_WINDOW_HPP
