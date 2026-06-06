#include "opengl_window.hpp"
#include "graph3d.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Cast helpers for internal pointers
#define DPY(p) static_cast<Display*>(p)
#define WIN(w) static_cast<Window>(w)
#define CTX(c) static_cast<GLXContext>(c)
#define ATM(a) static_cast<Atom>(a)

OpenGLWindow::OpenGLWindow() :
    paramA(1.0),
    paramB(1.0),
    paramC(0.0),
    timeVal(0.0),
    isAnimated(true),
    xMin(-10.0), xMax(10.0),
    yMin(-10.0), yMax(10.0),
    rotateX(30.0), rotateY(-45.0),
    zoomDistance(25.0),
    mousePressedLeft(false),
    mousePressedRight(false),
    lastMouseX(0),
    lastMouseY(0),
    displayPtr(nullptr),
    windowId(0),
    glContextPtr(nullptr),
    deleteMessageAtom(0),
    windowWidth(800),
    windowHeight(600) {}


OpenGLWindow::~OpenGLWindow() {
    cleanup();
}

bool OpenGLWindow::openWindow(const std::string& equationStr, bool is3D) {
    equation = equationStr;
    is3DMode = is3D;

    // Parse the equation once to verify it
    if (!parser.parse(equation)) {
        std::cerr << "Parser error: " << parser.getLastError() << std::endl;
        return false;
    }

    // Set initial parameter values based on the equation/mode
    paramA = 1.0;
    paramB = 1.0;
    paramC = 0.0;
    timeVal = 0.0;

    // Set viewport bounds based on mode
    if (equation == "mandelbrot" || equation == "julia" || equation == "sierpinski") {
        xMin = -2.0; xMax = 1.0;
        yMin = -1.2; yMax = 1.2;
    } else if (equation == "plasma") {
        xMin = -5.0; xMax = 5.0;
        yMin = -5.0; yMax = 5.0;
    } else if (equation == "clifford") {
        xMin = -2.5; xMax = 2.5;
        yMin = -2.5; yMax = 2.5;
    } else if (equation == "barnsley") {
        xMin = -3.0; xMax = 3.0;
        yMin = 0.0;  yMax = 10.0;
    } else if (equation == "lorenz") {
        is3DMode = true;
        xMin = -30.0; xMax = 30.0;
        yMin = -30.0; yMax = 30.0;
        zoomDistance = 60.0;
    } else if (equation == "aizawa") {
        is3DMode = true;
        xMin = -3.0; xMax = 3.0;
        yMin = -3.0; yMax = 3.0;
        zoomDistance = 6.0;
    } else if (is3DMode) {
        xMin = -10.0; xMax = 10.0;
        yMin = -10.0; yMax = 10.0;
    } else {
        xMin = -10.0; xMax = 10.0;
        yMin = -8.0;  yMax = 8.0;
    }

    if (!initX11GLX()) {
        std::cerr << "Failed to initialize X11/GLX Window\n";
        cleanup();
        return false;
    }

    runEventLoop();
    cleanup();
    return true;
}

bool OpenGLWindow::initX11GLX() {
    Display* dpy = XOpenDisplay(NULL);
    if (!dpy) {
        std::cerr << "Cannot connect to X server\n";
        return false;
    }
    displayPtr = dpy;

    int attrList[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        None
    };

    XVisualInfo* vi = glXChooseVisual(dpy, DefaultScreen(dpy), attrList);
    if (!vi) {
        // Fallback: try depth size 16
        int attrListFallback[] = {
            GLX_RGBA,
            GLX_DOUBLEBUFFER,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_DEPTH_SIZE, 16,
            None
        };
        vi = glXChooseVisual(dpy, DefaultScreen(dpy), attrListFallback);
        if (!vi) {
            std::cerr << "No appropriate visual found\n";
            return false;
        }
    }

    Colormap cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);

    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

    Window win = XCreateWindow(
        dpy, RootWindow(dpy, vi->screen),
        0, 0, windowWidth, windowHeight, 0,
        vi->depth, InputOutput, vi->visual,
        CWColormap | CWEventMask, &swa
    );
    windowId = win;

    // Set Window Title
    XStoreName(dpy, win, "GraphCalc - Interactive High-Res OpenGL Mode");

    // Intercept Window Close Button
    Atom wmDelete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wmDelete, 1);
    deleteMessageAtom = wmDelete;

    // Show window
    XMapWindow(dpy, win);

    // Create GLX Context
    GLXContext ctx = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    if (!ctx) {
        std::cerr << "Cannot create GLX context\n";
        XFree(vi);
        return false;
    }
    glContextPtr = ctx;

    glXMakeCurrent(dpy, win, ctx);
    XFree(vi);

    // Load X11 font for HUD display
    Font font = XLoadFont(dpy, "9x15");
    if (font) {
        glXUseXFont(font, 32, 96, 1000);
    }

    return true;
}

void OpenGLWindow::cleanup() {
    Display* dpy = DPY(displayPtr);
    GLXContext ctx = CTX(glContextPtr);
    Window win = WIN(windowId);

    if (ctx) {
        glXMakeCurrent(dpy, None, NULL);
        glXDestroyContext(dpy, ctx);
        glContextPtr = nullptr;
    }
    if (win) {
        XDestroyWindow(dpy, win);
        windowId = 0;
    }
    if (dpy) {
        XCloseDisplay(dpy);
        displayPtr = nullptr;
    }
}

void OpenGLWindow::runEventLoop() {
    Display* dpy = DPY(displayPtr);
    Window win = WIN(windowId);
    Atom wmDelete = ATM(deleteMessageAtom);

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (running) {
        // Handle all pending X11 Events
        while (XPending(dpy) > 0) {
            XEvent event;
            XNextEvent(dpy, &event);

            switch (event.type) {
                case ClientMessage: {
                    if (static_cast<Atom>(event.xclient.data.l[0]) == wmDelete) {
                        running = false;
                    }
                    break;
                }
                case ConfigureNotify: {
                    windowWidth = event.xconfigure.width;
                    windowHeight = event.xconfigure.height;
                    glViewport(0, 0, windowWidth, windowHeight);
                    break;
                }
                case ButtonPress: {
                    if (event.xbutton.button == Button1) {
                        mousePressedLeft = true;
                        lastMouseX = event.xbutton.x;
                        lastMouseY = event.xbutton.y;
                    } else if (event.xbutton.button == Button3) {
                        mousePressedRight = true;
                        lastMouseX = event.xbutton.x;
                        lastMouseY = event.xbutton.y;
                    } else if (event.xbutton.button == 4) { // Scroll Up (Zoom In)
                        if (is3DMode) {
                            zoomDistance = std::max(2.0, zoomDistance - 1.0);
                        } else {
                            double dx = (xMax - xMin) * 0.05;
                            double dy = (yMax - yMin) * 0.05;
                            xMin += dx; xMax -= dx;
                            yMin += dy; yMax -= dy;
                        }
                    } else if (event.xbutton.button == 5) { // Scroll Down (Zoom Out)
                        if (is3DMode) {
                            zoomDistance = std::min(500.0, zoomDistance + 1.0);
                        } else {
                            double dx = (xMax - xMin) * 0.05;
                            double dy = (yMax - yMin) * 0.05;
                            xMin -= dx; xMax += dx;
                            yMin -= dy; yMax += dy;
                        }
                    }
                    break;
                }
                case ButtonRelease: {
                    if (event.xbutton.button == Button1) mousePressedLeft = false;
                    if (event.xbutton.button == Button3) mousePressedRight = false;
                    break;
                }
                case MotionNotify: {
                    int dx = event.xmotion.x - lastMouseX;
                    int dy = event.xmotion.y - lastMouseY;
                    lastMouseX = event.xmotion.x;
                    lastMouseY = event.xmotion.y;

                    if (is3DMode) {
                        if (mousePressedLeft) {
                            rotateY += dx * 0.5;
                            rotateX += dy * 0.5;
                        } else if (mousePressedRight) {
                            zoomDistance += dy * 0.1;
                        }
                    } else { // 2D Drag-Panning
                        if (mousePressedLeft) {
                            double worldWidth = xMax - xMin;
                            double worldHeight = yMax - yMin;
                            double shiftX = (static_cast<double>(dx) / windowWidth) * worldWidth;
                            double shiftY = (static_cast<double>(dy) / windowHeight) * worldHeight;
                            xMin -= shiftX;
                            xMax -= shiftX;
                            yMin += shiftY;
                            yMax += shiftY;
                        }
                    }
                    break;
                }
                case KeyPress: {
                    KeySym sym = XLookupKeysym(&event.xkey, 0);
                    if (sym == XK_Escape || sym == XK_q || sym == XK_Q) {
                        running = false;
                    } 
                    // Animation toggle
                    else if (sym == XK_space) {
                        isAnimated = !isAnimated;
                    }
                    // Reset View
                    else if (sym == XK_r || sym == XK_R) {
                        if (is3DMode) {
                            rotateX = 30.0; rotateY = -45.0; zoomDistance = 25.0;
                        } else {
                            xMin = -10.0; xMax = 10.0; yMin = -8.0; yMax = 8.0;
                        }
                        paramA = 1.0; paramB = 1.0; paramC = 0.0;
                    }
                    // Parameter Adjustments:
                    // paramA (using A / Z)
                    else if (sym == XK_a || sym == XK_A) paramA += 0.05;
                    else if (sym == XK_z || sym == XK_Z) paramA -= 0.05;
                    // paramB (using S / X)
                    else if (sym == XK_s || sym == XK_S) paramB += 0.05;
                    else if (sym == XK_x || sym == XK_X) paramB -= 0.05;
                    // paramC (using D / C)
                    else if (sym == XK_d || sym == XK_D) paramC += 0.05;
                    else if (sym == XK_c || sym == XK_C) paramC -= 0.05;

                    break;
                }
            }
        }

        // Calculate delta-time and tick animation
        auto now = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(now - lastFrameTime).count();
        lastFrameTime = now;

        if (isAnimated) {
            timeVal += dt * 1.5;
        }

        // Render Frame
        renderFrame();

        // Swap buffers to display rendered content (vsynced if possible, otherwise sleep)
        glXSwapBuffers(dpy, win);

        // Sleep briefly to yield CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void OpenGLWindow::renderFrame() {
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f); // Sleek dark blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (is3DMode) {
        render3D();
    } else {
        render2D();
    }

    drawHUD();
}

void OpenGLWindow::render2D() {
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin, xMax, yMin, yMax, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (equation == "mandelbrot" || equation == "julia" || equation == "plasma" || equation == "sierpinski") {
        int gridW = 400;
        int gridH = 300;
        
        glPointSize(2.5f);
        glBegin(GL_POINTS);

        for (int y = 0; y < gridH; ++y) {
            double pyVal = yMin + (static_cast<double>(y) / (gridH - 1)) * (yMax - yMin);
            for (int x = 0; x < gridW; ++x) {
                double pxVal = xMin + (static_cast<double>(x) / (gridW - 1)) * (xMax - xMin);

                if (equation == "mandelbrot") {
                    double cr = pxVal;
                    double ci = pyVal;
                    double zr = 0.0;
                    double zi = 0.0;
                    int iter = 0;
                    int maxIter = 80;
                    while (zr*zr + zi*zi <= 4.0 && iter < maxIter) {
                        double temp = zr*zr - zi*zi + cr;
                        zi = 2.0*zr*zi + ci;
                        zr = temp;
                        iter++;
                    }
                    if (iter < maxIter) {
                        double norm = static_cast<double>(iter) / maxIter;
                        float r = 0.0f + norm * 0.8f;
                        float g = 0.9f - norm * 0.8f;
                        float b = 0.9f + norm * 0.05f;
                        glColor3f(r, g, b);
                        glVertex2f(pxVal, pyVal);
                    }
                } 
                else if (equation == "julia") {
                    double cr = -0.7 + 0.1 * paramA + 0.1 * std::cos(timeVal * 0.4);
                    double ci = 0.27015 + 0.1 * paramB + 0.1 * std::sin(timeVal * 0.4);
                    double zr = pxVal;
                    double zi = pyVal;
                    int iter = 0;
                    int maxIter = 80;
                    while (zr*zr + zi*zi <= 4.0 && iter < maxIter) {
                        double temp = zr*zr - zi*zi + cr;
                        zi = 2.0*zr*zi + ci;
                        zr = temp;
                        iter++;
                    }
                    if (iter < maxIter) {
                        double norm = static_cast<double>(iter) / maxIter;
                        float r = 0.2f + norm * 0.7f;
                        float g = 0.0f + norm * 0.9f;
                        float b = 0.8f - norm * 0.4f;
                        glColor3f(r, g, b);
                        glVertex2f(pxVal, pyVal);
                    }
                } 
                else if (equation == "plasma") {
                    double val = std::sin(pxVal * paramA + timeVal) +
                                 std::cos(pyVal * paramB - timeVal) +
                                 std::sin((pxVal + pyVal) * paramC + timeVal * 1.5) +
                                 std::cos(std::sqrt(pxVal*pxVal + pyVal*pyVal) * 2.0 - timeVal * 2.0);
                    double norm = (val + 4.0) / 8.0;
                    if (norm < 0.0) norm = 0.0;
                    if (norm > 1.0) norm = 1.0;
                    
                    float r = std::sin(norm * M_PI) * 0.8f + 0.2f;
                    float g = std::sin(norm * M_PI + 2.0*M_PI/3.0) * 0.8f + 0.2f;
                    float b = std::sin(norm * M_PI + 4.0*M_PI/3.0) * 0.8f + 0.2f;
                    glColor3f(r, g, b);
                    glVertex2f(pxVal, pyVal);
                }
                else if (equation == "sierpinski") {
                    bool inCarpet = true;
                    double tx = (pxVal + 2.0) / 2.7;
                    double ty = (pyVal + 1.2) / 2.4;
                    if (tx < 0.0 || tx > 1.0 || ty < 0.0 || ty > 1.0) inCarpet = false;
                    else {
                        for (int step = 0; step < 5; ++step) {
                            int ix = static_cast<int>(tx * 3.0);
                            int iy = static_cast<int>(ty * 3.0);
                            if (ix == 1 && iy == 1) {
                                inCarpet = false;
                                break;
                            }
                            tx = tx * 3.0 - ix;
                            ty = ty * 3.0 - iy;
                        }
                    }
                    if (inCarpet) {
                        glColor3f(0.0f, 0.95f, 0.5f);
                        glVertex2f(pxVal, pyVal);
                    }
                }
            }
        }
        glEnd();
        return;
    }
    else if (equation == "clifford") {
        glPointSize(1.5f);
        glBegin(GL_POINTS);
        
        double cx = 0.1, cy = 0.1;
        double a = -1.4 * paramA + 0.15 * std::sin(timeVal * 0.5);
        double b = 1.6 * paramB;
        double c = 1.0 + 0.1 * paramC;
        double d = 0.7 + 0.15 * std::cos(timeVal * 0.5);
        
        int numPoints = 15000;
        for (int i = 0; i < numPoints; ++i) {
            double nextX = std::sin(a * cy) + c * std::cos(a * cx);
            double nextY = std::sin(b * cx) + d * std::cos(b * cy);
            cx = nextX;
            cy = nextY;
            
            double norm = static_cast<double>(i) / numPoints;
            float r = 0.2f + 0.8f * std::sin(norm * M_PI);
            float g = 0.9f - 0.7f * norm;
            float b = 0.9f - 0.5f * std::cos(norm * M_PI);
            glColor3f(r, g, b);
            glVertex2f(cx, cy);
        }
        glEnd();
        return;
    }
    else if (equation == "barnsley") {
        glPointSize(1.5f);
        glBegin(GL_POINTS);
        
        double bx = 0.0, by = 0.0;
        
        uint32_t rng = 12345;
        auto next_rand = [&rng]() {
            rng = rng * 1664525 + 1013904223;
            return static_cast<double>(rng) / 4294967296.0;
        };

        int numPoints = 20000;
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

            float r_col = 0.0f + 0.1f * (i % 3);
            float g_col = 0.6f + 0.4f * std::sin(static_cast<double>(i) / 1000.0);
            float b_col = 0.1f + 0.2f * (i % 2);
            glColor3f(r_col, g_col, b_col);
            glVertex2f(tx, ty);
        }
        glEnd();
        return;
    }

    // 1. Draw Grid Lines
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // Grid color: dark gray-blue
    glColor3f(0.15f, 0.18f, 0.25f);
    
    // Vertical grid lines
    int startX = static_cast<int>(std::floor(xMin));
    int endX = static_cast<int>(std::ceil(xMax));
    for (int x = startX; x <= endX; x++) {
        if (x == 0) continue; // Skip axis
        glVertex2f(x, yMin);
        glVertex2f(x, yMax);
    }

    // Horizontal grid lines
    int startY = static_cast<int>(std::floor(yMin));
    int endY = static_cast<int>(std::ceil(yMax));
    for (int y = startY; y <= endY; y++) {
        if (y == 0) continue; // Skip axis
        glVertex2f(xMin, y);
        glVertex2f(xMax, y);
    }
    glEnd();

    // 2. Draw Axes (Thicker lines, light gray)
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.4f, 0.45f, 0.55f);
    // Y Axis (X = 0)
    if (xMin <= 0.0 && xMax >= 0.0) {
        glVertex2f(0.0, yMin);
        glVertex2f(0.0, yMax);
    }
    // X Axis (Y = 0)
    if (yMin <= 0.0 && yMax >= 0.0) {
        glVertex2f(xMin, 0.0);
        glVertex2f(xMax, 0.0);
    }
    glEnd();

    // 3. Render Equation Function (Bright Neon Cyan)
    glLineWidth(2.5f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0f, 0.95f, 0.95f);

    int numPoints = 1200;
    for (int i = 0; i <= numPoints; ++i) {
        double px = xMin + (i * (xMax - xMin) / numPoints);
        double py = parser.evaluate(px, 0.0, timeVal, paramA, paramB, paramC);
        
        // Clamping rendering coordinates slightly to prevent huge line overflow
        if (py >= yMin - 10.0 && py <= yMax + 10.0) {
            glVertex2f(px, py);
        }
    }
    glEnd();
    glDisable(GL_LINE_SMOOTH);
}

void OpenGLWindow::render3D() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Custom perspective projection using glFrustum
    double fov = 45.0;
    double aspect = static_cast<double>(windowWidth) / windowHeight;
    double zNear = 0.5;
    double zFar = 1000.0;
    double yMaxFrustum = zNear * std::tan(fov * M_PI / 360.0);
    double yMinFrustum = -yMaxFrustum;
    double xMaxFrustum = yMaxFrustum * aspect;
    double xMinFrustum = -xMaxFrustum;
    glFrustum(xMinFrustum, xMaxFrustum, yMinFrustum, yMaxFrustum, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Position and Rotate the Camera
    glTranslatef(0.0f, -2.0f, -zoomDistance);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

    if (equation == "lorenz") {
        double lx = 0.1, ly = 0.0, lz = 0.0;
        double sigma = 10.0;
        double rho = 28.0 * paramB;
        double beta = 8.0/3.0;
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

        int numPoints = 2500;
        std::vector<Point3D> points;
        for (int i = 0; i < numPoints; ++i) {
            double dx = sigma * (ly - lx);
            double dy = lx * (rho - lz) - ly;
            double dz = lx * ly - beta * lz;
            lx += dx * dt;
            ly += dy * dt;
            lz += dz * dt;
            points.push_back({lx, ly, lz - rho});
        }

        glLineWidth(2.0f);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < numPoints; ++i) {
            double norm = static_cast<double>(i) / numPoints;
            float r = 0.1f + norm * 0.9f;
            float g = 0.9f - norm * 0.8f;
            float b = 0.9f + norm * 0.1f;
            glColor3f(r, g, b);
            glVertex3f(points[i].x, points[i].y, points[i].z);
        }
        glEnd();
        glDisable(GL_LINE_SMOOTH);

        glColor3f(0.2f, 0.23f, 0.3f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        double boxSize = 25.0;
        for (int sx = -1; sx <= 1; sx += 2) {
            for (int sy = -1; sy <= 1; sy += 2) {
                glVertex3f(sx * boxSize, sy * boxSize, -boxSize);
                glVertex3f(sx * boxSize, sy * boxSize, boxSize);

                glVertex3f(sx * boxSize, -boxSize, sy * boxSize);
                glVertex3f(sx * boxSize, boxSize, sy * boxSize);

                glVertex3f(-boxSize, sx * boxSize, sy * boxSize);
                glVertex3f(boxSize, sx * boxSize, sy * boxSize);
            }
        }
        glEnd();
        return;
    }
    else if (equation == "aizawa") {
        double ax = 0.1, ay = 0.0, az = 0.0;
        double a = 0.95;
        double b = 0.7 * paramB;
        double c = 0.6;
        double d = 3.5 + paramC;
        double e = 0.25;
        double f = 0.1;
        double dt = 0.01 * paramA;
        if (dt < 0.001) dt = 0.001;

        for (int i = 0; i < 150; ++i) {
            double dx = (az - b) * ax - d * ay;
            double dy = d * ax + (az - b) * ay;
            double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
            ax += dx * dt;
            ay += dy * dt;
            az += dz * dt;
        }

        int numPoints = 3000;
        std::vector<Point3D> points;
        for (int i = 0; i < numPoints; ++i) {
            double dx = (az - b) * ax - d * ay;
            double dy = d * ax + (az - b) * ay;
            double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
            ax += dx * dt;
            ay += dy * dt;
            az += dz * dt;
            points.push_back({ax, ay, az - 1.2});
        }

        glLineWidth(2.0f);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < numPoints; ++i) {
            double norm = static_cast<double>(i) / numPoints;
            float r = 0.9f - norm * 0.8f;
            float g = 0.1f + norm * 0.8f;
            float b = 0.8f + norm * 0.2f;
            glColor3f(r, g, b);
            glVertex3f(points[i].x, points[i].y, points[i].z);
        }
        glEnd();
        glDisable(GL_LINE_SMOOTH);

        glColor3f(0.2f, 0.23f, 0.3f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        double boxSize = 2.0;
        for (int sx = -1; sx <= 1; sx += 2) {
            for (int sy = -1; sy <= 1; sy += 2) {
                glVertex3f(sx * boxSize, sy * boxSize, -boxSize);
                glVertex3f(sx * boxSize, sy * boxSize, boxSize);

                glVertex3f(sx * boxSize, -boxSize, sy * boxSize);
                glVertex3f(sx * boxSize, boxSize, sy * boxSize);

                glVertex3f(-boxSize, sx * boxSize, sy * boxSize);
                glVertex3f(boxSize, sx * boxSize, sy * boxSize);
            }
        }
        glEnd();
        return;
    }

    // Render wireframe surface
    int res = 60;
    double rangeX = xMax - xMin;
    double rangeY = yMax - yMin;

    // First, precalculate coordinates and values to find height range for colors
    std::vector<std::vector<double>> zValues(res, std::vector<double>(res));
    double minZ = 1e9, maxZ = -1e9;

    for (int i = 0; i < res; ++i) {
        double px = xMin + (i * rangeX / (res - 1));
        for (int j = 0; j < res; ++j) {
            double py = yMin + (j * rangeY / (res - 1));
            double pz = parser.evaluate(px, py, timeVal, paramA, paramB, paramC);
            zValues[i][j] = pz;
            if (pz < minZ) minZ = pz;
            if (pz > maxZ) maxZ = pz;
        }
    }

    if (maxZ == minZ) maxZ += 1.0;

    // Draw the 3D surface mesh
    glLineWidth(1.0f);

    // Color mapper based on height
    auto setVertexColor = [minZ, maxZ](double zVal) {
        double norm = (zVal - minZ) / (maxZ - minZ);
        // Neon Cyan-Purple gradient
        // Low values: Neon Cyan (0.0, 0.9, 0.9)
        // High values: Bright Purple (0.8, 0.1, 0.95)
        float r = 0.0f + norm * 0.8f;
        float g = 0.9f - norm * 0.8f;
        float b = 0.9f + norm * 0.05f;
        glColor3f(r, g, b);
    };

    // Draw line loops for surface mesh
    for (int i = 0; i < res; ++i) {
        double px1 = xMin + (i * rangeX / (res - 1));
        double px2 = (i + 1 < res) ? xMin + ((i + 1) * rangeX / (res - 1)) : px1;

        for (int j = 0; j < res; ++j) {
            double py1 = yMin + (j * rangeY / (res - 1));
            double py2 = (j + 1 < res) ? yMin + ((j + 1) * rangeY / (res - 1)) : py1;

            glBegin(GL_LINE_STRIP);
            // Connect to i+1
            if (i + 1 < res) {
                setVertexColor(zValues[i][j]);
                glVertex3f(px1, zValues[i][j], py1);
                setVertexColor(zValues[i+1][j]);
                glVertex3f(px2, zValues[i+1][j], py1);
            }
            glEnd();

            glBegin(GL_LINE_STRIP);
            // Connect to j+1
            if (j + 1 < res) {
                setVertexColor(zValues[i][j]);
                glVertex3f(px1, zValues[i][j], py1);
                setVertexColor(zValues[i][j+1]);
                glVertex3f(px1, zValues[i][j+1], py2);
            }
            glEnd();
        }
    }

    // Draw coordinate base grid in subtle gray
    glColor3f(0.2f, 0.23f, 0.3f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(xMin, minZ - 0.5, yMin);
    glVertex3f(xMax, minZ - 0.5, yMin);
    glVertex3f(xMax, minZ - 0.5, yMax);
    glVertex3f(xMin, minZ - 0.5, yMax);
    glEnd();
}

void OpenGLWindow::drawHUD() {
    // Save state
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render visual background panel for HUD (Translucent Dark Gray)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.02f, 0.02f, 0.05f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(10, windowHeight - 145);
    glVertex2f(380, windowHeight - 145);
    glVertex2f(380, windowHeight - 10);
    glVertex2f(10, windowHeight - 10);
    glEnd();
    glDisable(GL_BLEND);

    // Draw HUD Box border
    glLineWidth(1.5f);
    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(10, windowHeight - 145);
    glVertex2f(380, windowHeight - 145);
    glVertex2f(380, windowHeight - 10);
    glVertex2f(10, windowHeight - 10);
    glEnd();

    // Display Info text strings
    std::vector<std::string> lines;
    lines.push_back("GRAPHCALC REALTIME SIMULATION");
    lines.push_back(is3DMode ? "Equation: z = " + equation : "Equation: y = " + equation);
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Params: a = " << paramA << " | b = " << paramB << " | c = " << paramC;
    lines.push_back(ss.str());

    lines.push_back("Controls:");
    lines.push_back("  [A/Z] Adj 'a'  | [S/X] Adj 'b'  | [D/C] Adj 'c'");
    lines.push_back("  [Mouse Drag] Rotate/Pan | [Scroll] Zoom");
    lines.push_back("  [Space] Play/Pause Anim  | [R] Reset | [Esc/Q] Quit");

    // Drawing text list using X11 Glx font list base
    glColor3f(0.9f, 0.95f, 1.0f);
    for (size_t k = 0; k < lines.size(); ++k) {
        // Highlight header in cyan
        if (k == 0) {
            glColor3f(0.0f, 0.85f, 0.95f);
        } else if (k == 1) {
            glColor3f(0.0f, 0.95f, 0.6f);
        } else if (k == 3) {
            glColor3f(0.5f, 0.7f, 0.9f);
        } else {
            glColor3f(0.8f, 0.85f, 0.9f);
        }

        glRasterPos2i(20, windowHeight - 30 - (k * 18));
        glListBase(1000 - 32);
        glCallLists(lines[k].length(), GL_UNSIGNED_BYTE, lines[k].c_str());
    }

    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
