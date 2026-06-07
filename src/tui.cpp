#include "tui.hpp"
#include "plotter.hpp"
#include "opengl_window.hpp"
#include "graph3d.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>

// Helper to split a UTF-8 string into individual characters
static std::vector<std::string> splitUTF8(const std::string& str) {
    std::vector<std::string> chars;
    size_t i = 0;
    while (i < str.length()) {
        size_t len = 1;
        unsigned char c = str[i];
        if (c >= 0xf0) len = 4;
        else if (c >= 0xe0) len = 3;
        else if (c >= 0xc0) len = 2;
        
        if (i + len <= str.length()) {
            chars.push_back(str.substr(i, len));
        } else {
            chars.push_back(str.substr(i));
        }
        i += len;
    }
    return chars;
}

// Helper function to wrap text by word boundaries
static std::vector<std::string> wrapText(const std::string& text, int maxLen) {
    std::vector<std::string> lines;
    if (maxLen <= 0) {
        lines.push_back(text);
        return lines;
    }
    std::stringstream ss(text);
    std::string word;
    std::string currentLine = "";

    while (ss >> word) {
        if (currentLine.empty()) {
            currentLine = word;
        } else if (currentLine.length() + 1 + word.length() <= static_cast<size_t>(maxLen)) {
            currentLine += " " + word;
        } else {
            lines.push_back(currentLine);
            currentLine = word;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}

TUI::TUI() :
    selectedIndex(0),
    tabIndex(0),
    menuTimeVal(0.0),
    scrollOffset(0),
    maxScrollOffset(0),
    origTermiosPtr(nullptr) {

    // Get initial size
    updateTerminalSize();

    // 2D Templates
    templates2D = {
        {"Sine Wave (Animated)", "a * sin(b * x - t)", false, 
         R"(A fundamental periodic sinusoidal wave representing simple harmonic motion. The variable `a` scales the peak amplitude, `b` modifies the spatial frequency (angular frequency), and `t` acts as a time-based phase offset shifting the wave horizontally to simulate continuous movement. The sine wave traces its mathematical origin to ancient Indian astronomers (e.g., Aryabhata, c. 500 AD) and Greek chords. In the 1820s, Joseph Fourier proved that any periodic wave can be constructed from a sum of sines and cosines, establishing it as the fundamental building block of acoustics, signal processing, and electromagnetic wave theory.)"},
        {"Quadratic Parabola", "a * x^2 + b * x + c", false, 
         R"(A second-order polynomial curve representing a symmetric parabola. The parameter `a` dictates the direction of the opening (upwards if positive, downwards if negative) and vertical stretching, `b` shifts the vertex along a parabolic path, and `c` is the y-intercept. First systematically studied by Menaechmus in ancient Greece (4th century BC) as a conic section. In 1638, Galileo Galilei demonstrated that projectile trajectories under constant gravity follow a parabolic arc, making the parabola crucial in kinematics, mechanics, and telescope reflector design.)"},
        {"Dampened Wave", "exp(-a * x^2) * sin(b * x - t)", false, 
         R"(Represents a wave packet whose amplitude decays exponentially with the square of the distance from the origin. It is defined mathematically by the product of a Gaussian distribution envelope `exp(-a * x^2)` and a moving sinusoidal wave. Developed in classical mechanics in the 18th century to describe oscillating systems experiencing resistance, such as springs in viscous fluids or electrical currents in resistive circuits. It models energy dissipation and is fundamental to seismic wave analysis and acoustics.)"},
        {"Beating Wave", "sin(a * x) + cos(b * x)", false, 
         R"(The linear superposition of a sine wave and a cosine wave with distinct frequencies `a` and `b`. The mismatch in frequencies creates constructive and destructive interference, yielding a slow periodic modulation of the amplitude envelope. Explored by acoustics pioneers including Hermann von Helmholtz in "On the Sensations of Tone" (1863). Beats are heavily utilized in tuning musical instruments, radio heterodyning techniques, and analyzing wave-packet propagation in quantum wave mechanics.)"},
        {"Absolute Value", "a * abs(x) - b", false, 
         R"(A piecewise linear function forming a sharp, non-differentiable V-shaped corner at the origin. The slope of the two symmetric branches is controlled by the factor `a`, and the entire vertex is translated vertically downward by `b`. The absolute value function (or modulus) was formally defined by Karl Weierstrass in 1841. It is widely applied in optimization problems, mathematical distance metrics, and machine learning error metrics (such as Mean Absolute Error or L1 loss).)"},
        {"FM Synthesis Wave", "sin(a * x + b * sin(c * x - t))", false, 
         R"(Represents a frequency-modulated signal. The carrier frequency is scaled by `a`, the modulation index (intensity of the frequency deviation) is scaled by `b`, and the modulating wave's frequency and phase are determined by `c` and `t`. Developed in 1967 by John Chowning at Stanford University. Frequency Modulation (FM) synthesis transformed electronic music by allowing complex, harmonic and inharmonic sounds to be generated efficiently on digital hardware, leading to the landmark Yamaha DX7 synthesizer in 1983.)"},
        {"Fourier Approximation", "sin(x) + sin(3*x)/3 + sin(5*x)/5", false, 
         R"(The third-order expansion of the Fourier series representation of a square wave. The summation of odd harmonic sine waves, with amplitudes inversely proportional to their harmonic numbers, demonstrates the initial stages of convergence to a discontinuous square wave. Introduced by Joseph Fourier in 1807 within his study of heat diffusion. This approximation demonstrates the Gibbs phenomenon and forms the theoretical basis for digital audio compression (MP3, AAC) and electrical power harmonics analysis.)"},
        {"Sigmoid Curve", "a / (1 + exp(-b * x)) - c", false, 
         R"(A smooth, bounded S-shaped logistic curve. The parameter `a` scales the upper asymptote, `b` governs the steepness of the central transition region, and `c` translates the curve vertically. Originally formulated by Pierre François Verhulst in 1838 to model resource-limited population growth. In modern applications, it is the classic activation function in artificial neural networks and logistic regression, representing biological neuron firing thresholds.)"},
        {"Tangent Wave", "a * tan(b * x - t)", false, 
         R"(A periodic trigonometric function characterized by repeating vertical asymptotes occurring where the cosine term in its denominator becomes zero. The parameter `a` stretches the curve vertically, and `b` increases its spatial frequency. Originally utilized by medieval Islamic mathematicians (e.g., Abu al-Wafa, 10th century) to calculate shadows and angles. The tangent function is fundamental to map projections (Mercator projection), civil surveying, and poles/singularities analysis in complex math.)"},
        {"Gaussian Bell Curve", "a * exp(-b * x^2)", false, 
         R"(The standard normal distribution curve centered at zero. The peak amplitude is defined by `a`, and the width (variance) is inversely proportional to `b`. It decays rapidly to zero as x departs from the center. Discovered by Abraham de Moivre in 1738 and extensively developed by Carl Friedrich Gauss in 1809 for error analysis. It is the central curve of statistics, describing natural phenomena, measurement errors, and kinetic theory of gases.)"},
        {"Damped Cosine Wave", "a * exp(-abs(x)/2) * cos(b * x - t)", false, 
         R"(A cosine wave that decays symmetrically as it moves away from the origin in both positive and negative directions. The absolute value function `abs(x)` in the exponential exponent creates a sharp bilateral decay envelope. Simulates the impulse response of physical systems to a localized perturbation, such as a drop of water on a string. Used in signal processing to represent Gabor filters, which mimic biological visual receptive fields.)"}
    };

    // 3D Templates
    templates3D = {
        {"Ripple Waves", "a * sin(b * sqrt(x^2 + y^2) - t)", true, 
         R"(A three-dimensional radial wave where the height `z` is a function of the Euclidean distance `r = sqrt(x^2 + y^2)` from the origin. The wave propagates outward radially as time `t` increases. Solves the two-dimensional wave equation in polar coordinates using Bessel functions. It describes natural phenomena such as concentric ripples on a water surface and sound propagation from a spherical source.)"},
        {"Saddle (Hyperbolic Paraboloid)", "a * (x^2 - y^2) / 4", true, 
         R"(A doubly ruled quadratic surface that curves upwards along one principal axis and downwards along the other. The origin represents a saddle point (minimax point) where all first-order partial derivatives are zero. Studied during the rapid development of analytic geometry in the 17th century. Hyperbolic paraboloids are extensively used in structural architecture (e.g., saddle roofs) due to their strength and the ability to construct curved surfaces using straight beams.)"},
        {"Sombrero Function", "a * sin(b * sqrt(x^2 + y^2)) / (sqrt(x^2 + y^2) + 0.1)", true, 
         R"(A two-dimensional radial sinc-like function showing a prominent central peak at the origin, surrounded by decaying concentric circular ripples. The constant offset `0.1` avoids division-by-zero singularities. Represents the mathematical pattern of an Airy disk, named after George Biddell Airy (1835). This pattern is produced by the diffraction of light through a circular aperture, which is vital in optics, camera design, and astronomy limits.)"},
        {"Interfering Waves", "a * sin(b * x - t) * cos(c * y - t)", true, 
         R"(A dynamic three-dimensional surface formed by multiplying orthogonal traveling waves. The result is a repeating grid pattern of peaks, valleys, and saddle points that shift dynamically over time. Utilized to model two-dimensional standing waves within rectangular cavities or acoustic waveguides. This structure is foundational to the study of musical drumheads and electromagnetic resonance in microwave cavities.)"},
        {"Gaussian Wave Packet", "3 * exp(-(x^2 + y^2)/a) * cos(b * sqrt(x^2 + y^2) - t)", true, 
         R"(A localized three-dimensional wave packet. It is formed by multiplying a spatial Gaussian distribution envelope `exp(-r^2/a)` by a radial cosine wave, constraining the wave's amplitude near the origin. Introduced by Erwin Schrödinger in 1926 to model the behavior of localized quantum particles. It provides a visual representation of wave-particle duality and matches the intensity profile of focused laser beams.)"},
        {"Egg Carton Surface", "a * (cos(b * x) + cos(c * y))", true, 
         R"(A periodic heightmap created by summing two orthogonal cosine functions. It forms a grid of symmetric peaks, valleys, and saddle points reminiscent of commercial cardboard packaging. Employed in materials science to model the electrostatic potential energy surfaces of crystalline lattices (Lennard-Jones potentials). It is also used as a mathematical test function for multi-modal global optimization algorithms.)"},
        {"Diamond Pyramids", "a * (abs(x) + abs(y))", true, 
         R"(A continuous, non-differentiable heightmap composed of the sum of the absolute values of the coordinates. It creates a series of sharp, concentric diamond-shaped ridges sloping linearly outwards. Popularized in computer graphics and procedural game engine generation. This mathematical heightmap provides a simple method for rendering crystalline facets, ancient pyramids, and angular blocky structures in simulation environments.)"},
        {"Spiral Vortex", "a * sin(b * sqrt(x^2 + y^2) - c * atan(y/x) - t)", true, 
         R"(A dynamic rotating spiral surface. The wave's phase depends on both the radial distance `sqrt(x^2 + y^2)` and the polar angle `theta = atan(y/x)`. It rotates continuously around the central origin over time. Models spiral wave patterns found in biological tissues (e.g. cardiac tissue during fibrillation), excitable chemical media (the Belousov-Zhabotinsky reaction), and the structural spiral arms of galaxies.)"},
        {"Cone", "a * sqrt(x^2 + y^2)", true, 
         R"(A right circular cone whose height `z` is strictly proportional to the Euclidean distance from the origin. The surface has a constant slope `a` and exhibits a sharp, non-differentiable tip at the origin. Conic geometry was famously compiled by Apollonius of Perga around 200 BC. In modern physics, cones represent light cones in Einstein’s general relativity, defining causal boundaries in spacetime.)"},
        {"Chladni Plate Pattern", "a * (cos(b * x) * cos(c * y) - cos(c * x) * cos(b * y))", true, 
         R"(A wave equation solving for the resonant vibration modes of a thin square plate. Nodal lines (where `z = 0`) form intricate geometric boundaries that depend on the integer mode values `b` and `c`. Named after Ernst Chladni, who published "Discoveries in the Theory of Sound" in 1787. By scattering sand on vibrating metal plates, he visualized acoustic nodes, establishing the field of experimental acoustics.)"},
        {"Valleys and Hills", "a * sin(x) * cos(y)", true, 
         R"(A checkerboard pattern of alternating peaks and troughs generated by the product of sine and cosine. It is a smooth, continuous surface featuring symmetric saddle points between adjacent hills and valleys. Used in vector calculus textbooks since the 19th century to teach multidimensional optimization, partial derivatives, gradient vectors, and saddle point classification.)"}
    };

    // Visualisation Templates
    templatesVis = {
        {"Mandelbrot Set", "mandelbrot", false, 
         R"(The set of complex numbers `c` for which the sequence generated by quadratic iteration remains bounded. Colors represent the number of iterations required for the sequence to escape a radius of 2.0. First mapped mathematically by Robert Brooks and Peter Matelski in 1978, but visualized and popularized by Benoit Mandelbrot at IBM in 1980. It represents the ultimate icon of fractal geometry and chaos theory.)"},
        {"Julia Set (Morphing)", "julia", false, 
         R"(A fractal boundary in the complex plane. Unlike the Mandelbrot set, the parameter `c` is fixed (in this case, morphing dynamically over time `t`), and the pixel coordinates determine the initial seed `z_0`. Investigated by French mathematicians Gaston Julia and Pierre Fatou during World War I (c. 1918). It is a foundational structure in complex dynamics, illustrating chaos and infinite self-similarity.)"},
        {"Organic Plasma Pattern", "plasma", false, 
         R"(Combines linear and radial trigonometric waves dynamically. A multi-frequency sum of sine and cosine functions that outputs a shifting, organic pattern. By blending coordinate-based waves and radial distance-based waves, it produces fluid-like interference patterns. Developed by demoscene programmers in the 1980s and 1990s. This math was utilized to generate fluid, hallucinogenic background visuals on limited hardware (such as Amiga or MS-DOS PCs) using color-cycle lookups.)"},
        {"Lorenz Attractor (3D Chaotic)", "lorenz", true, 
         R"(A system of three coupled, non-linear ordinary differential equations. Under standard parameters, it generates a strange attractor with a fractional Hausdorff dimension of approximately 2.06. Formulated by meteorologist Edward Lorenz in 1963 to simplify atmospheric convection models. It became the hallmark of chaos theory and popularized the "butterfly effect"—where minute input differences cause massive output variations.)"},
        {"Sierpinski Carpet Fractal", "sierpinski", false, 
         R"(A planar fractal constructed by dividing a square into a 3x3 grid, removing the center sub-square, and recursively applying this procedure to the remaining eight sub-squares. Its Hausdorff dimension is `log(8)/log(3) ~ 1.89`. Described by Polish mathematician Wacław Sierpiński in 1916. Beyond topology, the Sierpinski carpet is utilized in engineering for fractal antennas, which reduce space while maintaining wideband performance.)"},
        {"Clifford Attractor", "clifford", false, 
         R"(A two-dimensional chaotic polynomial map. Iterating the discrete map generates a complex, dust-like strange attractor whose visual structure morphs depending on the constants `a`, `b`, `c`, and `d`. Named after Clifford Pickover, who introduced it in the early 1990s as a tool for generative mathematical art. It demonstrates how simple, discrete equations can produce highly detailed, organic-looking geometries.)"},
        {"Barnsley Fern Fractal", "barnsley", false, 
         R"(A fractal generated by the random iteration of four affine transformations. Each transformation contracts, rotates, and translates coordinates with a predefined probability, converging on a fern-leaf shape. Created by Michael Barnsley in 1988 ("Fractals Everywhere"). It proved that complex, organic structures in nature could be compressed into a tiny set of numbers, pioneering fractal-based image compression.)"},
        {"Aizawa Attractor (3D)", "aizawa", true, 
         R"(A system of three coupled non-linear differential equations that generates a chaotic trajectory orbiting a central axis, forming a twisting, spherical tube-like strange attractor. Discovered by Japanese physicist Yoji Aizawa in 1989 while researching conservative chaotic systems. It is popular in math art and visualization for its high level of visual symmetry and aesthetic quality.)"}
    };

    if (std::getenv("GRAPHCALC_TEST_SHORT_DESC")) {
        templates2D[4].description = "Short description.";
    }
}

TUI::~TUI() {
    restoreTerminal();
}

void TUI::updateTerminalSize() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        screenWidth = w.ws_col;
        screenHeight = w.ws_row;
    } else {
        screenWidth = 80;
        screenHeight = 26; // Default fallback
    }
    // Safety minimums
    if (screenWidth < 60) screenWidth = 60;
    if (screenHeight < 15) screenHeight = 15;
}

void TUI::clearGrid() {
    updateTerminalSize();
    screenGrid.assign(screenHeight, std::vector<TUICell>(screenWidth));
    for (int y = 0; y < screenHeight; ++y) {
        for (int x = 0; x < screenWidth; ++x) {
            screenGrid[y][x].ch = " ";
            screenGrid[y][x].color = "";
        }
    }
}

void TUI::drawText(int x, int y, const std::string& text, const std::string& color) {
    if (y < 0 || y >= screenHeight) return;
    auto chars = splitUTF8(text);
    for (size_t i = 0; i < chars.size(); ++i) {
        int curX = x + i;
        if (curX < 0 || curX >= screenWidth) continue;
        screenGrid[y][curX].ch = chars[i];
        screenGrid[y][curX].color = color;
    }
}

void TUI::drawBox(int x, int y, int w, int h, const std::string& color) {
    if (w <= 0 || h <= 0) return;
    
    // Draw corners
    drawText(x, y, "┌", color);
    drawText(x + w - 1, y, "┐", color);
    drawText(x, y + h - 1, "└", color);
    drawText(x + w - 1, y + h - 1, "┘", color);
    
    // Draw top and bottom borders
    for (int i = 1; i < w - 1; ++i) {
        drawText(x + i, y, "─", color);
        drawText(x + i, y + h - 1, "─", color);
    }
    // Draw left and right borders
    for (int j = 1; j < h - 1; ++j) {
        drawText(x, y + j, "│", color);
        drawText(x + w - 1, y + j, "│", color);
    }
}

void TUI::flushScreen() {
    std::stringstream ss;
    ss << "\033[H"; // Move cursor to top-left home position
    std::string lastColor = "";
    for (int y = 0; y < screenHeight; ++y) {
        for (int x = 0; x < screenWidth; ++x) {
            const auto& cell = screenGrid[y][x];
            if (cell.color != lastColor) {
                if (!lastColor.empty()) {
                    ss << "\033[0m";
                }
                ss << cell.color;
                lastColor = cell.color;
            }
            ss << cell.ch;
        }
        if (y < screenHeight - 1) {
            ss << "\n";
        }
    }
    ss << "\033[0m";
    std::cout << ss.str() << std::flush;
}


void TUI::renderMiniPreview(int startX, int startY, int w, int h, const std::string& eq, bool is3D, double time) {
    if (eq != currentMenuEquation || !currentMenuParser.isValid()) {
        currentMenuEquation = eq;
        if (!currentMenuParser.parse(eq)) {
            currentMenuEquation = "";
            return;
        }
    }

    Plotter plotter(w, h);

    if (eq == "mandelbrot" || eq == "julia" || eq == "plasma" || eq == "sierpinski") {
        int gridW = w * 2;
        int gridH = h * 4;
        for (int y = 0; y < gridH; ++y) {
            double pyVal = -1.2 + (static_cast<double>(y) / (gridH - 1)) * 2.4; // Range [-1.2, 1.2]
            for (int x = 0; x < gridW; ++x) {
                double pxVal = -2.0 + (static_cast<double>(x) / (gridW - 1)) * 2.7; // Range [-2.0, 0.7]

                if (eq == "mandelbrot") {
                    double cr = pxVal;
                    double ci = pyVal;
                    double zr = 0.0;
                    double zi = 0.0;
                    int iter = 0;
                    int maxIter = 24;
                    double zr2 = 0.0;
                    double zi2 = 0.0;
                    while (zr2 + zi2 <= 4.0 && iter < maxIter) {
                        zi = 2.0*zr*zi + ci;
                        zr = zr2 - zi2 + cr;
                        zr2 = zr*zr;
                        zi2 = zi*zi;
                        iter++;
                    }
                    if (iter < maxIter) {
                        int color = 40 + iter * 6;
                        if (color > 230) color = 230;
                        plotter.setDot(x, y, color);
                    }
                } 
                else if (eq == "julia") {
                    double jx = pxVal + 0.5;
                    double jy = pyVal;
                    double cr = -0.7 + 0.27015 * std::cos(time * 0.4);
                    double ci = 0.27015 + 0.3 * std::sin(time * 0.4);
                    double zr = jx;
                    double zi = jy;
                    int iter = 0;
                    int maxIter = 24;
                    double zr2 = zr*zr;
                    double zi2 = zi*zi;
                    while (zr2 + zi2 <= 4.0 && iter < maxIter) {
                        zi = 2.0*zr*zi + ci;
                        zr = zr2 - zi2 + cr;
                        zr2 = zr*zr;
                        zi2 = zi*zi;
                        iter++;
                    }
                    if (iter < maxIter) {
                        int color = 60 + iter * 7;
                        if (color > 230) color = 230;
                        plotter.setDot(x, y, color);
                    }
                } 
                else if (eq == "plasma") {
                    double val = std::sin(pxVal * 4.0 + time) +
                                 std::cos(pyVal * 4.0 - time) +
                                 std::sin((pxVal + pyVal) * 3.0 + time * 1.5) +
                                 std::cos(std::sqrt(pxVal*pxVal + pyVal*pyVal) * 5.0 - time * 2.0);
                    double norm = (val + 4.0) / 8.0;
                    if (norm < 0.0) norm = 0.0;
                    if (norm > 1.0) norm = 1.0;
                    if (norm > 0.55) {
                        int color = 50 + static_cast<int>(norm * 150.0);
                        if (color > 230) color = 230;
                        plotter.setDot(x, y, color);
                    }
                }
                else if (eq == "sierpinski") {
                    bool inCarpet = true;
                    double tx = (pxVal + 2.0) / 2.7;
                    double ty = (pyVal + 1.2) / 2.4;
                    if (tx < 0.0 || tx > 1.0 || ty < 0.0 || ty > 1.0) inCarpet = false;
                    else {
                        for (int step = 0; step < 4; ++step) {
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
                        plotter.setDot(x, y, 110);
                    }
                }
            }
        }
    }
    else if (eq == "clifford") {
        int gridW = w * 2;
        int gridH = h * 4;
        double cx = 0.1, cy = 0.1;
        double a = -1.4;
        double b = 1.6;
        double c = 1.0;
        double d = 0.7;
        
        for (int i = 0; i < 800; ++i) {
            double nextX = std::sin(a * cy) + c * std::cos(a * cx);
            double nextY = std::sin(b * cx) + d * std::cos(b * cy);
            cx = nextX;
            cy = nextY;
            
            int px = static_cast<int>((cx + 2.2) / 4.4 * gridW);
            int py = static_cast<int>((cy + 2.2) / 4.4 * gridH);
            if (px >= 0 && px < gridW && py >= 0 && py < gridH) {
                int color = 40 + (i * 180 / 800);
                plotter.setDot(px, py, color);
            }
        }
    }
    else if (eq == "barnsley") {
        int gridW = w * 2;
        int gridH = h * 4;
        double bx = 0.0, by = 0.0;
        
        uint32_t rng = 12345;
        auto next_rand = [&rng]() {
            rng = rng * 1664525 + 1013904223;
            return static_cast<double>(rng) / 4294967296.0;
        };

        for (int i = 0; i < 1500; ++i) {
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
            bx = nextX;
            by = nextY;
            
            int px = static_cast<int>((bx + 2.5) / 5.0 * gridW);
            int py = static_cast<int>(by / 10.0 * gridH);
            if (px >= 0 && px < gridW && py >= 0 && py < gridH) {
                plotter.setDot(px, py, 82);
            }
        }
    }
    else if (eq == "aizawa") {
        double ax = 0.1, ay = 0.0, az = 0.0;
        double a = 0.95;
        double b = 0.7;
        double c = 0.6;
        double d = 3.5;
        double e = 0.25;
        double f = 0.1;
        double dt = 0.01;

        for (int i = 0; i < 100; ++i) {
            double dx = (az - b) * ax - d * ay;
            double dy = d * ax + (az - b) * ay;
            double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
            ax += dx * dt;
            ay += dy * dt;
            az += dz * dt;
        }

        std::vector<Point3D> points;
        int numPoints = 250;
        for (int i = 0; i < numPoints; ++i) {
            double dx = (az - b) * ax - d * ay;
            double dy = d * ax + (az - b) * ay;
            double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
            ax += dx * dt;
            ay += dy * dt;
            az += dz * dt;
            points.push_back({ax * 1.5, ay * 1.5, (az - 1.2) * 1.5});
        }

        double angleX = 0.6;
        double angleY = 0.5 + time * 0.25;

        auto projectPt = [angleX, angleY](const Point3D& p, int gw, int gh) -> Point2D {
            double x = p.x;
            double y = p.y;
            double z = p.z;
            double tx = x * std::cos(angleY) + z * std::sin(angleY);
            double tz = -x * std::sin(angleY) + z * std::cos(angleY);
            x = tx; z = tz;
            double ty = y * std::cos(angleX) - z * std::sin(angleX);
            tz = y * std::sin(angleX) + z * std::cos(angleX);
            y = ty; z = tz;
            
            double fov = 40.0;
            double distance = 6.0;
            double denom = distance + z;
            if (denom < 0.1) denom = 0.1;
            double factor = fov / denom;
            int px = static_cast<int>(x * factor + gw / 2.0);
            int py = static_cast<int>(y * factor + gh / 2.0);
            return {px, py};
        };

        int gw = w * 2;
        int gh = h * 4;

        for (size_t i = 1; i < points.size(); ++i) {
            Point2D p1 = projectPt(points[i - 1], gw, gh);
            Point2D p2 = projectPt(points[i], gw, gh);
            int color = 60 + static_cast<int>((static_cast<double>(i) / points.size()) * 160.0);
            plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
        }
    }
    else if (eq == "lorenz") {
        double lx = 0.1, ly = 0.0, lz = 0.0;
        double sigma = 10.0;
        double rho = 28.0;
        double beta = 8.0/3.0;
        double dt = 0.015;

        std::vector<Point3D> points;
        for (int i = 0; i < 50; ++i) {
            double dx = sigma * (ly - lx);
            double dy = lx * (rho - lz) - ly;
            double dz = lx * ly - beta * lz;
            lx += dx * dt;
            ly += dy * dt;
            lz += dz * dt;
        }

        int numPoints = 150;
        for (int i = 0; i < numPoints; ++i) {
            double dx = sigma * (ly - lx);
            double dy = lx * (rho - lz) - ly;
            double dz = lx * ly - beta * lz;
            lx += dx * dt;
            ly += dy * dt;
            lz += dz * dt;
            points.push_back({lx * 0.4, ly * 0.4, (lz - 25.0) * 0.4});
        }

        double angleX = 0.6;
        double angleY = 0.5 + time * 0.25;

        auto projectPt = [angleX, angleY](const Point3D& p, int gw, int gh) -> Point2D {
            double x = p.x;
            double y = p.y;
            double z = p.z;
            double tx = x * std::cos(angleY) + z * std::sin(angleY);
            double tz = -x * std::sin(angleY) + z * std::cos(angleY);
            x = tx; z = tz;
            double ty = y * std::cos(angleX) - z * std::sin(angleX);
            tz = y * std::sin(angleX) + z * std::cos(angleX);
            y = ty; z = tz;
            
            double fov = 40.0;
            double distance = 16.0;
            double denom = distance + z;
            if (denom < 0.1) denom = 0.1;
            double factor = fov / denom;
            int px = static_cast<int>(x * factor + gw / 2.0);
            int py = static_cast<int>(y * factor + gh / 2.0);
            return {px, py};
        };

        int gw = w * 2;
        int gh = h * 4;

        for (size_t i = 1; i < points.size(); ++i) {
            Point2D p1 = projectPt(points[i - 1], gw, gh);
            Point2D p2 = projectPt(points[i], gw, gh);
            int color = 40 + static_cast<int>((static_cast<double>(i) / points.size()) * 180.0);
            plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
        }
    }
    else if (!is3D) {
        // Draw miniature 2D plot
        double xMin = -8.0, xMax = 8.0;
        double yMin = -4.0, yMax = 4.0;
        plotter.drawAxes(xMin, xMax, yMin, yMax);
        
        int gridW = w * 2;
        for (int i = 0; i < gridW; ++i) {
            double xVal = xMin + (static_cast<double>(i) / (gridW - 1)) * (xMax - xMin);
            double yVal = currentMenuParser.evaluate(xVal, 0.0, time, 1.0, 1.0, 0.0);
            if (yVal >= yMin && yVal <= yMax) {
                int py = static_cast<int>((yVal - yMin) / (yMax - yMin) * (h * 4 - 1));
                plotter.setDot(i, py, 0); // Cyan
            }
        }
    } 
    else {
        // Draw miniature 3D plot
        double xMin = -8.0, xMax = 8.0;
        double yMin = -8.0, yMax = 8.0;
        
        int res = 14;
        double rangeX = xMax - xMin;
        double rangeY = yMax - yMin;

        std::vector<std::vector<Point3D>> mesh;
        for (int i = 0; i < res; ++i) {
            std::vector<Point3D> row;
            double px = xMin + (i * rangeX / (res - 1));
            for (int j = 0; j < res; ++j) {
                double py = yMin + (j * rangeY / (res - 1));
                double pz = currentMenuParser.evaluate(px, py, time, 1.0, 1.0, 0.0);
                row.push_back({px, py, pz});
            }
            mesh.push_back(row);
        }

        double angleX = 0.6;
        double angleY = 0.5 + time * 0.2; // Slow rotate animation

        auto projectPt = [angleX, angleY](const Point3D& p, int gw, int gh) -> Point2D {
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
            // Perspective projection
            double fov = 40.0;
            double distance = 16.0;
            double denom = distance + z;
            if (denom < 0.1) denom = 0.1;
            double factor = fov / denom;
            int px = static_cast<int>(x * factor + gw / 2.0);
            int py = static_cast<int>(y * factor + gh / 2.0);
            return {px, py};
        };

        int gw = w * 2;
        int gh = h * 4;

        for (int i = 0; i < res; ++i) {
            for (int j = 0; j < res; ++j) {
                Point2D p1 = projectPt(mesh[i][j], gw, gh);
                
                int color = 40 + static_cast<int>((mesh[i][j].z + 3.0) / 6.0 * 160.0);
                if (color < 20) color = 20;
                if (color > 230) color = 230;

                if (i + 1 < res) {
                    Point2D p2 = projectPt(mesh[i+1][j], gw, gh);
                    plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                }
                if (j + 1 < res) {
                    Point2D p2 = projectPt(mesh[i][j+1], gw, gh);
                    plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                }
            }
        }
    }

    // Copy cells to screenGrid
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            auto cell = plotter.getCell(x, y);
            int targetX = startX + x;
            int targetY = startY + y;
            if (targetX >= 0 && targetX < screenWidth && targetY >= 0 && targetY < screenHeight) {
                screenGrid[targetY][targetX].ch = cell.first;
                screenGrid[targetY][targetX].color = cell.second;
            }

        }
    }
}

void TUI::enterAltBuffer() {
    std::cout << "\033[?1049h\033[H\033[?25l" << std::flush;
}

void TUI::exitAltBuffer() {
    std::cout << "\033[?1049l\033[?25h" << std::flush;
}

void TUI::setupTerminal() {
    struct termios* raw = new struct termios;
    tcgetattr(STDIN_FILENO, raw);
    origTermiosPtr = raw;

    struct termios t = *raw;
    t.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    t.c_cflag |= (CS8);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t);
}

void TUI::restoreTerminal() {
    if (origTermiosPtr) {
        struct termios* raw = static_cast<struct termios*>(origTermiosPtr);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, raw);
        delete raw;
        origTermiosPtr = nullptr;
    }
}

int TUI::readKey(bool nonBlocking) {
    char c;
    int nread;
    if (nonBlocking) {
        nread = read(STDIN_FILENO, &c, 1);
        if (nread <= 0) return 0; // No key pressed
    } else {
        while ((nread = read(STDIN_FILENO, &c, 1)) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (nread < 0) return -1;
    }

    if (c == 27) {
        char seq[3];
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int n1 = read(STDIN_FILENO, &seq[0], 1);
        if (n1 <= 0) return 27;
        
        int n2 = read(STDIN_FILENO, &seq[1], 1);
        if (n2 <= 0) return 27;

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return 1001; // Up Arrow
                case 'B': return 1002; // Down Arrow
                case 'C': return 1003; // Right Arrow
                case 'D': return 1004; // Left Arrow
            }
        }
        return 27;
    }
    return c;
}

void TUI::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void TUI::drawHeader() {
    // Left empty for compatibility, headers drawn directly in grid
}

void TUI::drawFooter(const std::string& info) {
    (void)info;
}


int TUI::run() {
    enterAltBuffer();
    setupTerminal();

    bool running = true;
    menuTimeVal = 0.0;
    while (running) {
        int prevTab = tabIndex;
        int prevSelected = selectedIndex;

        showMenu();
        int key = readKey(true); // Call readKey in non-blocking mode

        if (key > 0) {
            if (key == 'q' || key == 'Q' || key == 27) {
                running = false;
            } 
            else if (key == 1001) { // Up
                selectedIndex = std::max(0, selectedIndex - 1);
            } 
            else if (key == 1002) { // Down
                int maxIdx = (tabIndex == 0) ? templates2D.size() - 1 : (tabIndex == 1) ? templates3D.size() - 1 : templatesVis.size() - 1;
                selectedIndex = std::min(maxIdx, selectedIndex + 1);
            } 
            else if (key == 1003) { // Right Arrow
                tabIndex = (tabIndex + 1) % 3;
                selectedIndex = 0;
            } 
            else if (key == 1004) { // Left Arrow
                tabIndex = (tabIndex + 2) % 3;
                selectedIndex = 0;
            } 
            else if (key == '[') {
                scrollOffset = std::max(0, scrollOffset - 1);
            }
            else if (key == ']') {
                scrollOffset = std::min(maxScrollOffset, scrollOffset + 1);
            }
            else if (key == 'c' || key == 'C') { // Custom Input mode
                bool is3D = (tabIndex == 1);
                std::string customEq = handleCustomInput(is3D);
                if (!customEq.empty()) {
                    showSimulationMenu(customEq, is3D);
                }
            } 
            else if (key == 10 || key == 13) { // Enter
                const auto& list = (tabIndex == 0) ? templates2D : (tabIndex == 1) ? templates3D : templatesVis;
                showSimulationMenu(list[selectedIndex].equation, (tabIndex == 1));
            }
        }

        if (tabIndex != prevTab || selectedIndex != prevSelected) {
            scrollOffset = 0;
        }

        // Tick animations and sleep for 50ms (~20 FPS)
        menuTimeVal += 0.05;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    restoreTerminal();
    exitAltBuffer();
    return 0;
}

void TUI::showMenu() {
    clearGrid(); // Resizes screenGrid, fetches screenWidth and screenHeight

    // 1. Draw outer border box
    drawBox(0, 0, screenWidth, screenHeight, "\033[1;38;5;99m");

    // 2. Draw Header (centered)
    std::string headerStr = "G R A P H C A L C  --  M O D E R N  T E R M I N A L  P L O T T E R";
    int headerX = (screenWidth - headerStr.length()) / 2;
    if (headerX < 2) headerX = 2;
    drawText(headerX, 1, headerStr, "\033[1;38;5;51m");

    // 3. Tab headers
    if (tabIndex == 0) {
        drawText(2, 3, " [ 2D MODE ] ", "\033[1;38;5;220m");
        drawText(16, 3, "   3D MODE   ", "\033[38;5;244m");
        drawText(29, 3, "   VISUALS   ", "\033[38;5;244m");
    } else if (tabIndex == 1) {
        drawText(2, 3, "   2D MODE   ", "\033[38;5;244m");
        drawText(16, 3, " [ 3D MODE ] ", "\033[1;38;5;220m");
        drawText(29, 3, "   VISUALS   ", "\033[38;5;244m");
    } else {
        drawText(2, 3, "   2D MODE   ", "\033[38;5;244m");
        drawText(16, 3, "   3D MODE   ", "\033[38;5;244m");
        drawText(29, 3, " [ VISUALS ] ", "\033[1;38;5;220m");
    }

    // 4. Vertical dividing line
    int dividerX = 42; // Left column fixed width 42
    if (screenWidth < 90) {
        dividerX = screenWidth / 2;
    }
    for (int y = 3; y < screenHeight - 1; ++y) {
        drawText(dividerX, y, "│", "\033[38;5;244m");
    }

    // 5. Draw Left Column templates
    const auto& currentList = (tabIndex == 0) ? templates2D : (tabIndex == 1) ? templates3D : templatesVis;
    for (size_t i = 0; i < currentList.size(); ++i) {
        int rowY = 5 + i * 2;
        if (rowY >= screenHeight - 2) break; // Keep off instructions line
        
        std::string name = currentList[i].name;
        // Truncate template name if too long for left column
        if (static_cast<int>(name.length()) > dividerX - 6) {
            name = name.substr(0, dividerX - 9) + "...";
        }

        if (static_cast<int>(i) == selectedIndex) {
            drawText(2, rowY, "▶ " + name, "\033[1;38;5;51m");
        } else {
            drawText(4, rowY, name, "\033[38;5;250m");
        }
    }

    // 6. Draw Right Column Preview Section
    int rightColX = dividerX + 3;
    int rightColWidth = screenWidth - dividerX - 6;
    if (rightColWidth > 5) {
        drawText(rightColX + 1, 3, "L I V E  P R E V I E W", "\033[1;38;5;177m");
        
        // Preview box height scales dynamically with screen height
        int previewH = screenHeight - 15;
        if (previewH < 8) previewH = 8; // min height
        int previewW = rightColWidth;
        
        drawBox(rightColX, 4, previewW, previewH, "\033[38;5;244m");
        
        // Render mini preview inside box (w = previewW - 2, h = previewH - 2)
        renderMiniPreview(rightColX + 1, 5, previewW - 2, previewH - 2, currentList[selectedIndex].equation, (tabIndex == 1), menuTimeVal);

        // Metadata coordinates below preview box
        int descY = 4 + previewH + 1;
        if (descY < screenHeight - 2) {
            drawText(rightColX, descY, "Description:", "\033[1;38;5;220m");
            
            int baseDescContentY = descY + 1;
            bool showFormula = (screenHeight - 3 > 4 + previewH + 3);
            int limitY = showFormula ? (screenHeight - 5) : (screenHeight - 2);
            int visibleHeight = limitY - baseDescContentY + 1;
            if (visibleHeight < 0) visibleHeight = 0;

            std::string desc = currentList[selectedIndex].description;
            int maxLen = rightColWidth - 2;
            std::vector<std::string> descLines = wrapText(desc, maxLen);

            maxScrollOffset = std::max(0, static_cast<int>(descLines.size()) - visibleHeight);
            if (scrollOffset > maxScrollOffset) {
                scrollOffset = maxScrollOffset;
            }
            if (scrollOffset < 0) {
                scrollOffset = 0;
            }

            int linesDrawn = 0;
            for (int k = scrollOffset; k < static_cast<int>(descLines.size()) && linesDrawn < visibleHeight; ++k) {
                int lineY = baseDescContentY + linesDrawn;
                drawText(rightColX, lineY, descLines[k], "\033[38;5;250m");
                linesDrawn++;
            }

            if (static_cast<int>(descLines.size()) > visibleHeight && visibleHeight > 0) {
                int scrollbarX = rightColX + rightColWidth - 1;
                // Draw track
                for (int i = 0; i < visibleHeight; ++i) {
                    drawText(scrollbarX, baseDescContentY + i, "░", "\033[38;5;240m");
                }
                // Calculate thumb height
                int thumbHeight = (visibleHeight * visibleHeight) / static_cast<int>(descLines.size());
                if (thumbHeight < 1) thumbHeight = 1;
                if (thumbHeight >= visibleHeight) thumbHeight = visibleHeight - 1;
                if (thumbHeight < 1) thumbHeight = 1;

                // Calculate thumb start
                int scrollRange = static_cast<int>(descLines.size()) - visibleHeight;
                int thumbStart = 0;
                if (scrollRange > 0) {
                    thumbStart = (scrollOffset * (visibleHeight - thumbHeight)) / scrollRange;
                }

                // Draw thumb
                for (int i = 0; i < thumbHeight; ++i) {
                    drawText(scrollbarX, baseDescContentY + thumbStart + i, "█", "\033[1;38;5;51m");
                }
            }
        }

        int eqY = screenHeight - 3;
        if (eqY > 4 + previewH + 3) {
            drawText(rightColX, eqY - 1, "Formula:", "\033[1;38;5;177m");
            std::string eqDisplay;
            if (currentList[selectedIndex].equation == "mandelbrot") {
                eqDisplay = "z_n1 = z_n^2 + c";
            } else if (currentList[selectedIndex].equation == "julia") {
                eqDisplay = "z_n1 = z_n^2 + c (morphs over time)";
            } else if (currentList[selectedIndex].equation == "plasma") {
                eqDisplay = "f(x, y) = sin(x) + cos(y) + ...";
            } else if (currentList[selectedIndex].equation == "sierpinski") {
                eqDisplay = "Recursive 3x3 grid subdivision";
            } else if (currentList[selectedIndex].equation == "lorenz") {
                eqDisplay = "dx/dt = s*(y-x), dy/dt = x*(r-z)-y, dz/dt = x*y - b*z";
            } else if (currentList[selectedIndex].equation == "clifford") {
                eqDisplay = "x_n1 = sin(a*y) + c*cos(a*x), y_n1 = sin(b*x) + d*cos(b*y)";
            } else if (currentList[selectedIndex].equation == "barnsley") {
                eqDisplay = "IFS: x_n1 = a1*x + b1*y + c1, y_n1 = d1*x + e1*y + f1";
            } else if (currentList[selectedIndex].equation == "aizawa") {
                eqDisplay = "dx/dt = (z-b)*x - d*y, dy/dt = d*x + (z-b)*y, ...";
            } else {
                eqDisplay = (tabIndex == 1 ? "z = " : "y = ") + currentList[selectedIndex].equation;
            }
            if (eqDisplay.length() > static_cast<size_t>(rightColWidth)) {
                eqDisplay = eqDisplay.substr(0, rightColWidth - 3) + "...";
            }
            drawText(rightColX, eqY, eqDisplay, "\033[1;38;5;51m");
        }
    }

    // 7. Draw Bottom Instructions Box
    std::string footerStr = " [Left/Right] Mode | [Up/Down] Navigate | [Enter] Select | [C] Custom | [Q] Quit ";
    int footerX = (screenWidth - footerStr.length()) / 2;
    if (footerX < 2) footerX = 2;
    drawText(footerX, screenHeight - 1, footerStr, "\033[38;5;244m");

    // 8. Output grid
    flushScreen();
}


std::string TUI::handleCustomInput(bool is3D) {
    // Enable cursor for text input
    std::cout << "\033[?25h" << std::flush;

    std::string inputStr = "";
    std::vector<std::string> suggestionTemplates = {
        "sin(a*x - t)",
        "cos(a*x - t)",
        "a*x^2 + b*x + c",
        "exp(-a*x^2) * sin(b*x - t)",
        "sqrt(a^2 - x^2)",
        "3*sin(sqrt(x^2+y^2) - t)",
        "a*(x^2 - y^2) / 4",
        "5*sin(sqrt(x^2+y^2)) / (sqrt(x^2+y^2) + 0.1)",
        "a*sin(x - t)*cos(y - t)"
    };

    while (true) {
        clearScreen();
        std::cout << "\033[1;38;5;99m========================================================================\n"
                  << "     G R A P H C A L C  --  M O D E R N  T E R M I N A L  P L O T T E R\n"
                  << "========================================================================\033[0m\n\n";
        std::cout << "\033[1;38;5;177mEnter custom equation (" << (is3D ? "z = f(x, y)" : "y = f(x)") << "):\033[0m\n\n";
        std::cout << "  > " << "\033[1;38;5;51m" << inputStr << "\033[0m";

        // Generate Auto-Suggestion
        std::string suggestionSuffix = "";
        std::string fullSuggestion = "";
        if (!inputStr.empty()) {
            for (const auto& temp : suggestionTemplates) {
                std::string cleanTemp = temp;
                cleanTemp.erase(std::remove(cleanTemp.begin(), cleanTemp.end(), ' '), cleanTemp.end());
                std::string cleanInput = inputStr;
                cleanInput.erase(std::remove(cleanInput.begin(), cleanInput.end(), ' '), cleanInput.end());

                if (cleanTemp.rfind(cleanInput, 0) == 0) {
                    suggestionSuffix = temp.substr(inputStr.length());
                    fullSuggestion = temp;
                    break;
                }
            }
        }

        if (!suggestionSuffix.empty()) {
            std::cout << "\033[38;5;244m" << suggestionSuffix << "\033[0m";
        }

        std::cout << "\033[" << (5 + inputStr.length()) << "G" << std::flush;

        int key = readKey(false); // Call readKey in blocking mode here

        if (key == 27) { // Escape
            std::cout << "\033[?25l" << std::flush;
            return "";
        } 
        else if (key == 10 || key == 13) { // Enter
            if (inputStr.empty()) continue;
            
            MathParser tempParser;
            if (tempParser.parse(inputStr)) {
                std::cout << "\033[?25l" << std::flush;
                return inputStr;
            } else {
                std::cout << "\n\n  \033[1;31mError parsing equation: " << tempParser.getLastError() << "\033[0m\n";
                std::cout << "  Press any key to retry...";
                std::cout << std::flush;
                readKey(false);
            }
        } 
        else if (key == 127 || key == 8) { // Backspace
            if (!inputStr.empty()) {
                inputStr.pop_back();
            }
        } 
        else if (key == 9 || key == 1003) { // Tab or Right Arrow
            if (!fullSuggestion.empty()) {
                inputStr = fullSuggestion;
            }
        } 
        else if (key >= 32 && key <= 126) { // Printable
            inputStr += static_cast<char>(key);
        }
    }
}


void TUI::showSimulationMenu(const std::string& equation, bool is3D) {
    int simChoice = 0;
    bool inMenu = true;
    while (inMenu) {
        clearGrid();

        // 1. Draw outer border box
        drawBox(0, 0, screenWidth, screenHeight, "\033[1;38;5;99m");

        // 2. Draw Header
        std::string headerStr = "Choose Visualization Mode";
        int headerX = (screenWidth - headerStr.length()) / 2;
        if (headerX < 2) headerX = 2;
        drawText(headerX, 1, headerStr, "\033[1;38;5;51m");

        // 3. Draw Equation Details
        std::string eqDisplay = std::string("Equation: ") + (is3D ? "z = " : "y = ") + equation;
        int eqX = (screenWidth - eqDisplay.length()) / 2;
        if (eqX < 2) eqX = 2;
        drawText(eqX, 3, eqDisplay, "\033[1;38;5;177m");

        // 4. Calculate layout for the windows
        int footerY = screenHeight - 1;
        int goBackH = 3;
        int goBackY = footerY - goBackH - 1;
        int winY = 5;
        int winH = goBackY - winY - 1;
        if (winH < 4) winH = 4; // safety minimum
        int winW = (screenWidth - 6) / 2;
        if (winW < 20) winW = 20;

        int win1X = 2;
        int win2X = win1X + winW + 2;

        // Border colors based on selection
        std::string col1 = (simChoice == 0) ? "\033[1;38;5;51m" : "\033[38;5;244m";
        std::string col2 = (simChoice == 1) ? "\033[1;38;5;51m" : "\033[38;5;244m";
        std::string col3 = (simChoice == 2) ? "\033[1;38;5;51m" : "\033[38;5;244m";

        // Draw Left Window (Terminal View)
        drawBox(win1X, winY, winW, winH, col1);
        std::string title1 = "[ 1 ] TERMINAL PLOTTER";
        int title1X = win1X + (winW - title1.length()) / 2;
        drawText(title1X, winY + 1, title1, (simChoice == 0) ? "\033[1;38;5;220m" : "\033[38;5;250m");

        // Info inside Left Window
        std::vector<std::string> lines1 = {
            "Renders the graph inside your",
            "terminal window using high-res",
            "Unicode Braille characters.",
            "",
            "• Responsive to window resizing",
            "• Interactive parameter tuning",
            "• Mouse drag support in TUI",
            "",
            "Press [1] or Enter to Select"
        };
        for (size_t i = 0; i < lines1.size(); ++i) {
            int lineY = winY + 3 + i;
            if (lineY >= winY + winH - 1) break;
            std::string text = lines1[i];
            if (static_cast<int>(text.length()) > winW - 4) {
                text = text.substr(0, winW - 7) + "...";
            }
            int textX = win1X + (winW - text.length()) / 2;
            drawText(textX, lineY, text, "\033[38;5;250m");
        }

        // Draw Right Window (OpenGL View)
        drawBox(win2X, winY, winW, winH, col2);
        std::string title2 = "[ 2 ] HIGH-RES OPENGL";
        int title2X = win2X + (winW - title2.length()) / 2;
        drawText(title2X, winY + 1, title2, (simChoice == 1) ? "\033[1;38;5;220m" : "\033[38;5;250m");

        // Info inside Right Window
        std::vector<std::string> lines2 = {
            "Opens a dedicated X11 OpenGL",
            "window for ultra-smooth 60 FPS",
            "hardware-accelerated rendering.",
            "",
            "• Detailed 3D mesh projection",
            "• Dynamic color gradients",
            "• Mouse rotation and zooming",
            "",
            "Press [2] or Enter to Select"
        };
        for (size_t i = 0; i < lines2.size(); ++i) {
            int lineY = winY + 3 + i;
            if (lineY >= winY + winH - 1) break;
            std::string text = lines2[i];
            if (static_cast<int>(text.length()) > winW - 4) {
                text = text.substr(0, winW - 7) + "...";
            }
            int textX = win2X + (winW - text.length()) / 2;
            drawText(textX, lineY, text, "\033[38;5;250m");
        }

        // Draw Go Back Button
        std::string title3 = "[ 3 ] GO BACK";
        int win3W = 20;
        int win3X = (screenWidth - win3W) / 2;
        drawBox(win3X, goBackY, win3W, goBackH, col3);
        int title3X = win3X + (win3W - title3.length()) / 2;
        drawText(title3X, goBackY + 1, title3, (simChoice == 2) ? "\033[1;38;5;220m" : "\033[38;5;250m");

        // 5. Draw Footer
        std::string footerStr = " [Left/Right/Up/Down] Navigate | [1/2/3] Select | [Enter] Launch | [Q/Esc] Go Back ";
        int footerX = (screenWidth - footerStr.length()) / 2;
        if (footerX < 2) footerX = 2;
        drawText(footerX, screenHeight - 1, footerStr, "\033[38;5;244m");

        // Flush
        flushScreen();

        // Key reading
        int key = readKey();
        if (key > 0) {
            if (key == 'q' || key == 'Q' || key == 27) {
                inMenu = false;
            }
            else if (key == 1004) { // Left
                if (simChoice == 1) {
                    simChoice = 0;
                } else if (simChoice == 2) {
                    simChoice = 0;
                }
            }
            else if (key == 1003) { // Right
                if (simChoice == 0) {
                    simChoice = 1;
                } else if (simChoice == 2) {
                    simChoice = 1;
                }
            }
            else if (key == 1002) { // Down
                if (simChoice == 0 || simChoice == 1) {
                    simChoice = 2;
                }
            }
            else if (key == 1001) { // Up
                if (simChoice == 2) {
                    simChoice = 0;
                }
            }
            else if (key == '1') {
                simChoice = 0;
                runTerminalSimulation(equation, is3D);
            }
            else if (key == '2') {
                simChoice = 1;
                restoreTerminal();
                std::cout << "\033[?25h" << std::flush;
                OpenGLWindow glWin;
                glWin.openWindow(equation, is3D);
                std::cout << "\033[?25l" << std::flush;
                setupTerminal();
            }
            else if (key == '3') {
                simChoice = 2;
                inMenu = false;
            }
            else if (key == 10 || key == 13) { // Enter
                if (simChoice == 0) {
                    runTerminalSimulation(equation, is3D);
                } else if (simChoice == 1) {
                    restoreTerminal();
                    std::cout << "\033[?25h" << std::flush;
                    OpenGLWindow glWin;
                    glWin.openWindow(equation, is3D);
                    std::cout << "\033[?25l" << std::flush;
                    setupTerminal();
                } else if (simChoice == 2) {
                    inMenu = false;
                }
            }
        }
    }
}

void TUI::runTerminalSimulation(const std::string& equation, bool is3D) {
    MathParser simParser;
    if (!simParser.parse(equation)) {
        return;
    }

    double paramA = 1.0;
    double paramB = 1.0;
    double paramC = 0.0;
    double timeVal = 0.0;
    bool isAnimated = true;

    // View boundaries
    double xMin = -10.0, xMax = 10.0;
    double yMin = -10.0, yMax = 10.0;

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
    }

    // 3D rotation params
    double angleX = 0.6;
    double angleY = 0.5;

    auto lastTime = std::chrono::high_resolution_clock::now();

    // Enable SGR Mouse tracking reporting
    std::cout << "\033[?1002h\033[?1006h" << std::flush;

    bool running = true;
    while (running) {
        // Clear terminal display and draw plot
        std::cout << "\033[2J\033[H";

        updateTerminalSize();
        int width = screenWidth - 2;
        int height = screenHeight - 5;
        if (width < 20) width = 20;
        if (height < 5) height = 5;

        double cosY = std::cos(angleY);
        double sinY = std::sin(angleY);
        double cosX = std::cos(angleX);
        double sinX = std::sin(angleX);

        Plotter plotter(width, height);

        if (equation == "mandelbrot" || equation == "julia" || equation == "plasma" || equation == "sierpinski") {
            int gridW = width * 2;
            int gridH = height * 4;
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
                        int maxIter = 30;
                        double zr2 = 0.0;
                        double zi2 = 0.0;
                        while (zr2 + zi2 <= 4.0 && iter < maxIter) {
                            zi = 2.0*zr*zi + ci;
                            zr = zr2 - zi2 + cr;
                            zr2 = zr*zr;
                            zi2 = zi*zi;
                            iter++;
                        }
                        if (iter < maxIter) {
                            int color = 40 + iter * 5;
                            if (color > 230) color = 230;
                            plotter.setDot(x, y, color);
                        }
                    } 
                    else if (equation == "julia") {
                        double cr = -0.7 + 0.1 * paramA + 0.15 * std::cos(timeVal * 0.4);
                        double ci = 0.27015 + 0.1 * paramB + 0.15 * std::sin(timeVal * 0.4);
                        double zr = pxVal;
                        double zi = pyVal;
                        int iter = 0;
                        int maxIter = 30;
                        double zr2 = zr*zr;
                        double zi2 = zi*zi;
                        while (zr2 + zi2 <= 4.0 && iter < maxIter) {
                            zi = 2.0*zr*zi + ci;
                            zr = zr2 - zi2 + cr;
                            zr2 = zr*zr;
                            zi2 = zi*zi;
                            iter++;
                        }
                        if (iter < maxIter) {
                            int color = 60 + iter * 5;
                            if (color > 230) color = 230;
                            plotter.setDot(x, y, color);
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
                        if (norm > 0.5) {
                            int color = 50 + static_cast<int>(norm * 160.0);
                            if (color > 230) color = 230;
                            plotter.setDot(x, y, color);
                        }
                    }
                    else if (equation == "sierpinski") {
                        bool inCarpet = true;
                        double tx = (pxVal + 2.0) / 2.7;
                        double ty = (pyVal + 1.2) / 2.4;
                        if (tx < 0.0 || tx > 1.0 || ty < 0.0 || ty > 1.0) inCarpet = false;
                        else {
                            for (int step = 0; step < 4; ++step) {
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
                            plotter.setDot(x, y, 110);
                        }
                    }
                }
            }
        }
        else if (equation == "clifford") {
            int gridW = width * 2;
            int gridH = height * 4;
            double cx = 0.1, cy = 0.1;
            double a = -1.4 * paramA + 0.15 * std::sin(timeVal * 0.5);
            double b = 1.6 * paramB;
            double c = 1.0 + 0.1 * paramC;
            double d = 0.7 + 0.15 * std::cos(timeVal * 0.5);
            
            for (int i = 0; i < 1500; ++i) {
                double nextX = std::sin(a * cy) + c * std::cos(a * cx);
                double nextY = std::sin(b * cx) + d * std::cos(b * cy);
                cx = nextX;
                cy = nextY;
                
                int px = static_cast<int>((cx - xMin) / (xMax - xMin) * gridW);
                int py = static_cast<int>((cy - yMin) / (yMax - yMin) * gridH);
                if (px >= 0 && px < gridW && py >= 0 && py < gridH) {
                    int color = 40 + (i * 180 / 1500);
                    plotter.setDot(px, py, color);
                }
            }
        }
        else if (equation == "barnsley") {
            int gridW = width * 2;
            int gridH = height * 4;
            double bx = 0.0, by = 0.0;
            
            uint32_t rng = 12345;
            auto next_rand = [&rng]() {
                rng = rng * 1664525 + 1013904223;
                return static_cast<double>(rng) / 4294967296.0;
            };

            for (int i = 0; i < 3000; ++i) {
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
                
                int px = static_cast<int>((tx - xMin) / (xMax - xMin) * gridW);
                int py = static_cast<int>((ty - yMin) / (yMax - yMin) * gridH);
                if (px >= 0 && px < gridW && py >= 0 && py < gridH) {
                    int color = 82 + (i % 5) * 6;
                    if (color > 255) color = 255;
                    plotter.setDot(px, py, color);
                }
            }
        }
        else if (!is3D) {
            // Draw axis
            plotter.drawAxes(xMin, xMax, yMin, yMax);
            
            // Plot 2D line
            int gridW = width * 2;
            for (int i = 0; i < gridW; ++i) {
                double xVal = xMin + (static_cast<double>(i) / (gridW - 1)) * (xMax - xMin);
                double yVal = simParser.evaluate(xVal, 0.0, timeVal, paramA, paramB, paramC);
                if (yVal >= yMin && yVal <= yMax) {
                    int py = static_cast<int>((yVal - yMin) / (yMax - yMin) * (height * 4 - 1));
                    plotter.setDot(i, py, 0); // cyan
                }
            }
        } 
        else {
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

                std::vector<Point3D> points;
                int numPoints = 250;
                for (int i = 0; i < numPoints; ++i) {
                    double dx = sigma * (ly - lx);
                    double dy = lx * (rho - lz) - ly;
                    double dz = lx * ly - beta * lz;
                    lx += dx * dt;
                    ly += dy * dt;
                    lz += dz * dt;
                    points.push_back({lx, ly, lz - rho});
                }

                auto projectPt = [cosX, sinX, cosY, sinY](const Point3D& p, int gw, int gh) -> Point2D {
                    double x = p.x;
                    double y = p.y;
                    double z = p.z;
                    double tx = x * cosY + z * sinY;
                    double tz = -x * sinY + z * cosY;
                    x = tx; z = tz;
                    double ty = y * cosX - z * sinX;
                    tz = y * sinX + z * cosX;
                    y = ty; z = tz;
                    
                    double fov = 100.0;
                    double distance = 40.0;
                    double denom = distance + z;
                    if (denom < 0.1) denom = 0.1;
                    double factor = fov / denom;
                    int px = static_cast<int>(x * factor + gw / 2.0);
                    int py = static_cast<int>(y * factor + gh / 2.0);
                    return {px, py};
                };

                int gw = width * 2;
                int gh = height * 4;

                for (size_t i = 1; i < points.size(); ++i) {
                    Point2D p1 = projectPt(points[i - 1], gw, gh);
                    Point2D p2 = projectPt(points[i], gw, gh);
                    int color = 40 + static_cast<int>((static_cast<double>(i) / points.size()) * 180.0);
                    plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                }
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

                for (int i = 0; i < 100; ++i) {
                    double dx = (az - b) * ax - d * ay;
                    double dy = d * ax + (az - b) * ay;
                    double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
                    ax += dx * dt;
                    ay += dy * dt;
                    az += dz * dt;
                }

                std::vector<Point3D> points;
                int numPoints = 350;
                for (int i = 0; i < numPoints; ++i) {
                    double dx = (az - b) * ax - d * ay;
                    double dy = d * ax + (az - b) * ay;
                    double dz = c + a * az - (az * az * az) / 3.0 - (ax * ax + ay * ay) * (1.0 + e * az) + f * az * ax * ax * ax;
                    ax += dx * dt;
                    ay += dy * dt;
                    az += dz * dt;
                    points.push_back({ax * 1.5, ay * 1.5, (az - 1.2) * 1.5});
                }

                auto projectPt = [cosX, sinX, cosY, sinY](const Point3D& p, int gw, int gh) -> Point2D {
                    double x = p.x;
                    double y = p.y;
                    double z = p.z;
                    double tx = x * cosY + z * sinY;
                    double tz = -x * sinY + z * cosY;
                    x = tx; z = tz;
                    double ty = y * cosX - z * sinX;
                    tz = y * sinX + z * cosX;
                    y = ty; z = tz;
                    
                    double fov = 100.0;
                    double distance = 6.0;
                    double denom = distance + z;
                    if (denom < 0.1) denom = 0.1;
                    double factor = fov / denom;
                    int px = static_cast<int>(x * factor + gw / 2.0);
                    int py = static_cast<int>(y * factor + gh / 2.0);
                    return {px, py};
                };

                int gw = width * 2;
                int gh = height * 4;

                for (size_t i = 1; i < points.size(); ++i) {
                    Point2D p1 = projectPt(points[i - 1], gw, gh);
                    Point2D p2 = projectPt(points[i], gw, gh);
                    int color = 60 + static_cast<int>((static_cast<double>(i) / points.size()) * 160.0);
                    plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                }
            }
            else {
                // Plot 3D surface projected onto 2D terminal canvas
                int res = 20; // smaller grid for terminal resolution (optimized from 24)
                double px_range = xMax - xMin;
                double py_range = yMax - yMin;

                // Generate points & project
                std::vector<std::vector<Point3D>> mesh;
                for (int i = 0; i < res; ++i) {
                    std::vector<Point3D> row;
                    double px = xMin + (i * px_range / (res - 1));
                    for (int j = 0; j < res; ++j) {
                        double py = yMin + (j * py_range / (res - 1));
                        double pz = simParser.evaluate(px, py, timeVal, paramA, paramB, paramC);
                        row.push_back({px, py, pz});
                    }
                    mesh.push_back(row);
                }

                // Projection math
                auto projectPt = [cosX, sinX, cosY, sinY](const Point3D& p, int gw, int gh) -> Point2D {
                    double x = p.x;
                    double y = p.y;
                    double z = p.z;
                    // Rotate Y
                    double tx = x * cosY + z * sinY;
                    double tz = -x * sinY + z * cosY;
                    x = tx; z = tz;
                    // Rotate X
                    double ty = y * cosX - z * sinX;
                    tz = y * sinX + z * cosX;
                    y = ty; z = tz;
                    // Perspective projection
                    double fov = 100.0;
                    double distance = 15.0;
                    double denom = distance + z;
                    if (denom < 0.1) denom = 0.1;
                    double factor = fov / denom;
                    int px = static_cast<int>(x * factor + gw / 2.0);
                    int py = static_cast<int>(y * factor + gh / 2.0);
                    return {px, py};
                };

                int gw = width * 2;
                int gh = height * 4;

                for (int i = 0; i < res; ++i) {
                    for (int j = 0; j < res; ++j) {
                        Point2D p1 = projectPt(mesh[i][j], gw, gh);
                        
                        // Color code vertex based on Z height (map -5 to 5 to colors 30 to 220)
                        int color = 40 + static_cast<int>((mesh[i][j].z + 4.0) / 8.0 * 180.0);
                        if (color < 20) color = 20;
                        if (color > 230) color = 230;

                        if (i + 1 < res) {
                            Point2D p2 = projectPt(mesh[i+1][j], gw, gh);
                            plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                        }
                        if (j + 1 < res) {
                            Point2D p2 = projectPt(mesh[i][j+1], gw, gh);
                            plotter.drawLine(p1.x, p1.y, p2.x, p2.y, color);
                        }
                    }
                }
            }
        }

        std::cout << plotter.render();
        std::cout << "SIMULATING: \033[1;38;5;51m" << (is3D ? "z = " : "y = ") << equation << "\033[0m\n";
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "a = " << paramA << " | b = " << paramB << " | c = " << paramC;
        std::cout << "\033[1;38;5;177m" << ss.str() << "\033[0m\n";
        std::cout << "\033[38;5;244m[A/Z] Adj 'a' | [S/X] Adj 'b' | [D/C] Adj 'c' | [Space] Play/Pause | [Q] Quit\033[0m\n";

        // Read input non-blocking
        char c;
        int n = read(STDIN_FILENO, &c, 1);
        if (n > 0) {
            if (c == 'q' || c == 'Q') {
                running = false;
            } 
            else if (c == ' ') {
                isAnimated = !isAnimated;
            } 
            else if (c == 'a' || c == 'A') paramA += 0.1;
            else if (c == 'z' || c == 'Z') paramA -= 0.1;
            else if (c == 's' || c == 'S') paramB += 0.1;
            else if (c == 'x' || c == 'X') paramB -= 0.1;
            else if (c == 'd' || c == 'D') paramC += 0.1;
            else if (c == 'c' || c == 'C') paramC -= 0.1;
            else if (c == 'r' || c == 'R') {
                paramA = 1.0; paramB = 1.0; paramC = 0.0;
            }

            else if (c == 27) { // Esc or Mouse Sequence
                char seq[3];
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                if (read(STDIN_FILENO, &seq[0], 1) > 0 && read(STDIN_FILENO, &seq[1], 1) > 0) {
                    if (seq[0] == '[' && seq[1] == '<') {
                        // SGR mouse read until M/m
                        std::string mouseStr = "";
                        char mc;
                        while (read(STDIN_FILENO, &mc, 1) > 0) {
                            mouseStr += mc;
                            if (mc == 'M' || mc == 'm') break;
                        }
                        // Parse values
                        int mbtn = 0, mx = 0, my = 0;
                        std::stringstream mss(mouseStr.substr(0, mouseStr.length() - 1));
                        char msep;
                        mss >> mbtn >> msep >> mx >> msep >> my;
                        // Handle left click/drag parameter adjust in TUI
                        if (is3D && (mbtn == 0 || mbtn == 32)) {
                            // Rotate 3D plot based on mouse dragging position in terminal!
                            angleY = (static_cast<double>(mx) / width) * 2.0 * M_PI;
                            angleX = (static_cast<double>(my) / height) * M_PI;
                        } else if (!is3D && (mbtn == 0 || mbtn == 32)) {
                            // Map mouse movement to adjusting a & b coefficients in 2D
                            paramA = (static_cast<double>(mx) / width) * 4.0 - 2.0;
                            paramB = (static_cast<double>(my) / height) * 4.0 - 2.0;
                        }
                    }
                } else {
                    running = false;
                }
            }
        }

        // Time updates
        auto now = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(now - lastTime).count();
        lastTime = now;
        if (isAnimated) {
            timeVal += dt * 1.5;
        }

        // Framerate limit (30 FPS)
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    // Disable SGR Mouse reporting
    std::cout << "\033[?1002l\033[?1006l" << std::flush;
}
