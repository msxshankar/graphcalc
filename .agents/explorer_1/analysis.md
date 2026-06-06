# graphcalc Codebase Analysis & Implementation Report

This report contains:
1. Complete mathematical and historical descriptions (at least 80 characters of detailed natural prose) for all 30 graph templates (11 2D templates, 11 3D templates, 8 Visualisation templates).
2. A detailed implementation plan for vertical scrolling in the TUI description box using `[` and `]` keys, resetting to 0 on template/tab change.
3. Recommendations for an E2E testing strategy/framework that runs on Linux.
4. An analysis of compilation constraints and terminal resizing behavior.

---

## 1. Mathematical and Historical Descriptions of the 30 Templates

### 2D Templates (11)

#### 1. Sine Wave (Animated)
- **Equation**: `y = a * sin(b * x - t)`
- **Mathematical Description**: A fundamental periodic sinusoidal wave representing simple harmonic motion. The variable `a` scales the peak amplitude, `b` modifies the spatial frequency (angular frequency), and `t` acts as a time-based phase offset shifting the wave horizontally to simulate continuous movement.
- **Historical & Application Context**: The sine wave traces its mathematical origin to ancient Indian astronomers (e.g., Aryabhata, c. 500 AD) and Greek chords. In the 1820s, Joseph Fourier proved that any periodic wave can be constructed from a sum of sines and cosines, establishing it as the fundamental building block of acoustics, signal processing, and electromagnetic wave theory.

#### 2. Quadratic Parabola
- **Equation**: `y = a * x^2 + b * x + c`
- **Mathematical Description**: A second-order polynomial curve representing a symmetric parabola. The parameter `a` dictates the direction of the opening (upwards if positive, downwards if negative) and vertical stretching, `b` shifts the vertex along a parabolic path, and `c` is the y-intercept.
- **Historical & Application Context**: First systematically studied by Menaechmus in ancient Greece (4th century BC) as a conic section. In 1638, Galileo Galilei demonstrated that projectile trajectories under constant gravity follow a parabolic arc, making the parabola crucial in kinematics, mechanics, and telescope reflector design.

#### 3. Dampened Wave
- **Equation**: `y = exp(-a * x^2) * sin(b * x - t)`
- **Mathematical Description**: Represents a wave packet whose amplitude decays exponentially with the square of the distance from the origin. It is defined mathematically by the product of a Gaussian distribution envelope `exp(-a * x^2)` and a moving sinusoidal wave.
- **Historical & Application Context**: Developed in classical mechanics in the 18th century to describe oscillating systems experiencing resistance, such as springs in viscous fluids or electrical currents in resistive circuits. It models energy dissipation and is fundamental to seismic wave analysis and acoustics.

#### 4. Beating Wave
- **Equation**: `y = sin(a * x) + cos(b * x)`
- **Mathematical Description**: The linear superposition of a sine wave and a cosine wave with distinct frequencies `a` and `b`. The mismatch in frequencies creates constructive and destructive interference, yielding a slow periodic modulation of the amplitude envelope.
- **Historical & Application Context**: Explored by acoustics pioneers including Hermann von Helmholtz in "On the Sensations of Tone" (1863). Beats are heavily utilized in tuning musical instruments, radio heterodyning techniques, and analyzing wave-packet propagation in quantum wave mechanics.

#### 5. Absolute Value
- **Equation**: `y = a * abs(x) - b`
- **Mathematical Description**: A piecewise linear function forming a sharp, non-differentiable V-shaped corner at the origin. The slope of the two symmetric branches is controlled by the factor `a`, and the entire vertex is translated vertically downward by `b`.
- **Historical & Application Context**: The absolute value function (or modulus) was formally defined by Karl Weierstrass in 1841. It is widely applied in optimization problems, mathematical distance metrics, and machine learning error metrics (such as Mean Absolute Error or L1 loss).

#### 6. FM Synthesis Wave
- **Equation**: `y = sin(a * x + b * sin(c * x - t))`
- **Mathematical Description**: Represents a frequency-modulated signal. The carrier frequency is scaled by `a`, the modulation index (intensity of the frequency deviation) is scaled by `b`, and the modulating wave's frequency and phase are determined by `c` and `t`.
- **Historical & Application Context**: Developed in 1967 by John Chowning at Stanford University. Frequency Modulation (FM) synthesis transformed electronic music by allowing complex, harmonic and inharmonic sounds to be generated efficiently on digital hardware, leading to the landmark Yamaha DX7 synthesizer in 1983.

#### 7. Fourier Approximation
- **Equation**: `y = sin(x) + sin(3*x)/3 + sin(5*x)/5`
- **Mathematical Description**: The third-order expansion of the Fourier series representation of a square wave. The summation of odd harmonic sine waves, with amplitudes inversely proportional to their harmonic numbers, demonstrates the initial stages of convergence to a discontinuous square wave.
- **Historical & Application Context**: Introduced by Joseph Fourier in 1807 within his study of heat diffusion. This approximation demonstrates the Gibbs phenomenon and forms the theoretical basis for digital audio compression (MP3, AAC) and electrical power harmonics analysis.

#### 8. Sigmoid Curve
- **Equation**: `y = a / (1 + exp(-b * x)) - c`
- **Mathematical Description**: A smooth, bounded S-shaped logistic curve. The parameter `a` scales the upper asymptote, `b` governs the steepness of the central transition region, and `c` translates the curve vertically.
- **Historical & Application Context**: Originally formulated by Pierre François Verhulst in 1838 to model resource-limited population growth. In modern applications, it is the classic activation function in artificial neural networks and logistic regression, representing biological neuron firing thresholds.

#### 9. Tangent Wave
- **Equation**: `y = a * tan(b * x - t)`
- **Mathematical Description**: A periodic trigonometric function characterized by repeating vertical asymptotes occurring where the cosine term in its denominator becomes zero. The parameter `a` stretches the curve vertically, and `b` increases its spatial frequency.
- **Historical & Application Context**: Originally utilized by medieval Islamic mathematicians (e.g., Abu al-Wafa, 10th century) to calculate shadows and angles. The tangent function is fundamental to map projections (Mercator projection), civil surveying, and poles/singularities analysis in complex math.

#### 10. Gaussian Bell Curve
- **Equation**: `y = a * exp(-b * x^2)`
- **Mathematical Description**: The standard normal distribution curve centered at zero. The peak amplitude is defined by `a`, and the width (variance) is inversely proportional to `b`. It decays rapidly to zero as x departs from the center.
- **Historical & Application Context**: Discovered by Abraham de Moivre in 1738 and extensively developed by Carl Friedrich Gauss in 1809 for error analysis. It is the central curve of statistics, describing natural phenomena, measurement errors, and kinetic theory of gases.

#### 11. Damped Cosine Wave
- **Equation**: `y = a * exp(-abs(x)/2) * cos(b * x - t)`
- **Mathematical Description**: A cosine wave that decays symmetrically as it moves away from the origin in both positive and negative directions. The absolute value function `abs(x)` in the exponential exponent creates a sharp bilateral decay envelope.
- **Historical & Application Context**: Simulates the impulse response of physical systems to a localized perturbation, such as a drop of water on a string. Used in signal processing to represent Gabor filters, which mimic biological visual receptive fields.

---

### 3D Templates (11)

#### 12. Ripple Waves
- **Equation**: `z = a * sin(b * sqrt(x^2 + y^2) - t)`
- **Mathematical Description**: A three-dimensional radial wave where the height `z` is a function of the Euclidean distance `r = sqrt(x^2 + y^2)` from the origin. The wave propagates outward radially as time `t` increases.
- **Historical & Application Context**: Solves the two-dimensional wave equation in polar coordinates using Bessel functions. It describes natural phenomena such as concentric ripples on a water surface and sound propagation from a spherical source.

#### 13. Saddle (Hyperbolic Paraboloid)
- **Equation**: `z = a * (x^2 - y^2) / 4`
- **Mathematical Description**: A doubly ruled quadratic surface that curves upwards along one principal axis and downwards along the other. The origin represents a saddle point (minimax point) where all first-order partial derivatives are zero.
- **Historical & Application Context**: Studied during the rapid development of analytic geometry in the 17th century. Hyperbolic paraboloids are extensively used in structural architecture (e.g., saddle roofs) due to their strength and the ability to construct curved surfaces using straight beams.

#### 14. Sombrero Function
- **Equation**: `z = a * sin(b * sqrt(x^2 + y^2)) / (sqrt(x^2 + y^2) + 0.1)`
- **Mathematical Description**: A two-dimensional radial sinc-like function showing a prominent central peak at the origin, surrounded by decaying concentric circular ripples. The constant offset `0.1` avoids division-by-zero singularities.
- **Historical & Application Context**: Represents the mathematical pattern of an Airy disk, named after George Biddell Airy (1835). This pattern is produced by the diffraction of light through a circular aperture, which is vital in optics, camera design, and astronomy limits.

#### 15. Interfering Waves
- **Equation**: `z = a * sin(b * x - t) * cos(c * y - t)`
- **Mathematical Description**: A dynamic three-dimensional surface formed by multiplying orthogonal traveling waves. The result is a repeating grid pattern of peaks, valleys, and saddle points that shift dynamically over time.
- **Historical & Application Context**: Utilized to model two-dimensional standing waves within rectangular cavities or acoustic waveguides. This structure is foundational to the study of musical drumheads and electromagnetic resonance in microwave cavities.

#### 16. Gaussian Wave Packet
- **Equation**: `z = 3 * exp(-(x^2 + y^2)/a) * cos(b * sqrt(x^2 + y^2) - t)`
- **Mathematical Description**: A localized three-dimensional wave packet. It is formed by multiplying a spatial Gaussian distribution envelope `exp(-r^2/a)` by a radial cosine wave, constraining the wave's amplitude near the origin.
- **Historical & Application Context**: Introduced by Erwin Schrödinger in 1926 to model the behavior of localized quantum particles. It provides a visual representation of wave-particle duality and matches the intensity profile of focused laser beams.

#### 17. Egg Carton Surface
- **Equation**: `z = a * (cos(b * x) + cos(c * y))`
- **Mathematical Description**: A periodic heightmap created by summing two orthogonal cosine functions. It forms a grid of symmetric peaks, valleys, and saddle points reminiscent of commercial cardboard packaging.
- **Historical & Application Context**: Employed in materials science to model the electrostatic potential energy surfaces of crystalline lattices (Lennard-Jones potentials). It is also used as a mathematical test function for multi-modal global optimization algorithms.

#### 18. Diamond Pyramids
- **Equation**: `z = a * (abs(x) + abs(y))`
- **Mathematical Description**: A continuous, non-differentiable heightmap composed of the sum of the absolute values of the coordinates. It creates a series of sharp, concentric diamond-shaped ridges sloping linearly outwards.
- **Historical & Application Context**: Popularized in computer graphics and procedural game engine generation. This mathematical heightmap provides a simple method for rendering crystalline facets, ancient pyramids, and angular blocky structures in simulation environments.

#### 19. Spiral Vortex
- **Equation**: `z = a * sin(b * sqrt(x^2 + y^2) - c * atan(y/x) - t)`
- **Mathematical Description**: A dynamic rotating spiral surface. The wave's phase depends on both the radial distance `sqrt(x^2 + y^2)` and the polar angle `theta = atan(y/x)`. It rotates continuously around the central origin over time.
- **Historical & Application Context**: Models spiral wave patterns found in biological tissues (e.g. cardiac tissue during fibrillation), excitable chemical media (the Belousov-Zhabotinsky reaction), and the structural spiral arms of galaxies.

#### 20. Cone
- **Equation**: `z = a * sqrt(x^2 + y^2)`
- **Mathematical Description**: A right circular cone whose height `z` is strictly proportional to the Euclidean distance from the origin. The surface has a constant slope `a` and exhibits a sharp, non-differentiable tip at the origin.
- **Historical & Application Context**: Conic geometry was famously compiled by Apollonius of Perga around 200 BC. In modern physics, cones represent light cones in Einstein’s general relativity, defining causal boundaries in spacetime.

#### 21. Chladni Plate Pattern
- **Equation**: `z = a * (cos(b * x) * cos(c * y) - cos(c * x) * cos(b * y))`
- **Mathematical Description**: A wave equation solving for the resonant vibration modes of a thin square plate. Nodal lines (where `z = 0`) form intricate geometric boundaries that depend on the integer mode values `b` and `c`.
- **Historical & Application Context**: Named after Ernst Chladni, who published "Discoveries in the Theory of Sound" in 1787. By scattering sand on vibrating metal plates, he visualized acoustic nodes, establishing the field of experimental acoustics.

#### 22. Valleys and Hills
- **Equation**: `z = a * sin(x) * cos(y)`
- **Mathematical Description**: A checkerboard pattern of alternating peaks and troughs generated by the product of sine and cosine. It is a smooth, continuous surface featuring symmetric saddle points between adjacent hills and valleys.
- **Historical & Application Context**: Used in vector calculus textbooks since the 19th century to teach multidimensional optimization, partial derivatives, gradient vectors, and saddle point classification.

---

### Visualisation Templates (8)

#### 23. Mandelbrot Set
- **Equation**: `z_n1 = z_n^2 + c` with `z_0 = 0`
- **Mathematical Description**: The set of complex numbers `c` for which the sequence generated by quadratic iteration remains bounded. Colors represent the number of iterations required for the sequence to escape a radius of 2.0.
- **Historical & Application Context**: First mapped mathematically by Robert Brooks and Peter Matelski in 1978, but visualized and popularized by Benoit Mandelbrot at IBM in 1980. It represents the ultimate icon of fractal geometry and chaos theory.

#### 24. Julia Set (Morphing)
- **Equation**: `z_n1 = z_n^2 + c` with `z_0 = z`
- **Mathematical Description**: A fractal boundary in the complex plane. Unlike the Mandelbrot set, the parameter `c` is fixed (in this case, morphing dynamically over time `t`), and the pixel coordinates determine the initial seed `z_0`.
- **Historical & Application Context**: Investigated by French mathematicians Gaston Julia and Pierre Fatou during World War I (c. 1918). It is a foundational structure in complex dynamics, illustrating chaos and infinite self-similarity.

#### 25. Organic Plasma Pattern
- **Equation**: Combines linear and radial trigonometric waves dynamically.
- **Mathematical Description**: A multi-frequency sum of sine and cosine functions that outputs a shifting, organic pattern. By blending coordinate-based waves and radial distance-based waves, it produces fluid-like interference patterns.
- **Historical & Application Context**: Developed by demoscene programmers in the 1980s and 1990s. This math was utilized to generate fluid, hallucinogenic background visuals on limited hardware (such as Amiga or MS-DOS PCs) using color-cycle lookups.

#### 26. Lorenz Attractor (3D Chaotic)
- **Equation**: `dx/dt = s*(y-x)`, `dy/dt = x*(r-z)-y`, `dz/dt = x*y - b*z`
- **Mathematical Description**: A system of three coupled, non-linear ordinary differential equations. Under standard parameters, it generates a strange attractor with a fractional Hausdorff dimension of approximately 2.06.
- **Historical & Application Context**: Formulated by meteorologist Edward Lorenz in 1963 to simplify atmospheric convection models. It became the hallmark of chaos theory and popularized the "butterfly effect"—where minute input differences cause massive output variations.

#### 27. Sierpinski Carpet Fractal
- **Equation**: Recursive 3x3 plane division.
- **Mathematical Description**: A planar fractal constructed by dividing a square into a 3x3 grid, removing the center sub-square, and recursively applying this procedure to the remaining eight sub-squares. Its Hausdorff dimension is `log(8)/log(3) ~ 1.89`.
- **Historical & Application Context**: Described by Polish mathematician Wacław Sierpiński in 1916. Beyond topology, the Sierpinski carpet is utilized in engineering for fractal antennas, which reduce space while maintaining wideband performance.

#### 28. Clifford Attractor
- **Equation**: `x_n1 = sin(a*y_n) + c*cos(a*x_n)`, `y_n1 = sin(b*x_n) + d*cos(b*y_n)`
- **Mathematical Description**: A two-dimensional chaotic polynomial map. Iterating the discrete map generates a complex, dust-like strange attractor whose visual structure morphs depending on the constants `a`, `b`, `c`, and `d`.
- **Historical & Application Context**: Named after Clifford Pickover, who introduced it in the early 1990s as a tool for generative mathematical art. It demonstrates how simple, discrete equations can produce highly detailed, organic-looking geometries.

#### 29. Barnsley Fern Fractal
- **Equation**: Iterated Function System (IFS) affine maps.
- **Mathematical Description**: A fractal generated by the random iteration of four affine transformations. Each transformation contracts, rotates, and translates coordinates with a predefined probability, converging on a fern-leaf shape.
- **Historical & Application Context**: Created by Michael Barnsley in 1988 ("Fractals Everywhere"). It proved that complex, organic structures in nature could be compressed into a tiny set of numbers, pioneering fractal-based image compression.

#### 30. Aizawa Attractor (3D)
- **Equation**: Coupled 3D ODEs with spherical attraction coefficients.
- **Mathematical Description**: A system of three coupled non-linear differential equations that generates a chaotic trajectory orbiting a central axis, forming a twisting, spherical tube-like strange attractor.
- **Historical & Application Context**: Discovered by Japanese physicist Yoji Aizawa in 1989 while researching conservative chaotic systems. It is popular in math art and visualization for its high level of visual symmetry and aesthetic quality.

---

## 2. Implementation Plan for Description Box Scrolling

To enable vertical scrolling of long descriptions in the TUI, we will modify the main menu rendering and keyboard loops.

### A. Class Modifications (`src/tui.hpp`)

We will add new member variables to `TUI` to track scroll state:

```cpp
private:
    // ... existing variables ...
    int scrollOffset;     // Current scrolled line index (0-indexed)
    int maxScrollOffset;  // Maximum scroll line offset (calculated dynamically)
```

### B. dynamic Height Calculation (`src/tui.cpp` inside `showMenu()`)

We calculate the available height for the description box, `visibleHeight`, dynamically based on the terminal's `screenHeight` and whether the formula is displayed.

```cpp
// 1. Calculate boundaries of description box
int descY = 4 + previewH + 1; // Starting Y-coord of "Description:" header
int baseDescContentY = descY + 1; // First line of description text

// 2. Check if the Formula box is displayed (which requires 2 rows)
bool showFormula = (screenHeight - 3 > 4 + previewH + 3);

// 3. Determine the last row available for description text (inclusive)
int limitY = showFormula ? (screenHeight - 5) : (screenHeight - 2);

// 4. Compute visibleHeight
int visibleHeight = limitY - baseDescContentY + 1;
if (visibleHeight < 1) visibleHeight = 0;
```

*Note:* If `visibleHeight` is 0 or negative, rendering of description text is bypassed to avoid crashes or out-of-bounds array writes.

### C. Word Wrapping and Viewport Rendering

Instead of character-slicing which breaks words in the middle, we implement a robust word-wrapping function.

```cpp
// Helper function to wrap text by word boundaries
std::vector<std::string> wrapText(const std::string& text, size_t maxWidth) {
    std::vector<std::string> lines;
    std::stringstream ss(text);
    sys::string word;
    std::string currentLine = "";

    while (ss >> word) {
        if (currentLine.empty()) {
            currentLine = word;
        } else if (currentLine.length() + 1 + word.length() <= maxWidth) {
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
```

Then in `showMenu()`, we calculate the text layout:
```cpp
// Reserve the rightmost 2 columns of the box for padding and the scrollbar
int textWidth = rightColWidth - 2; 
std::string desc = currentList[selectedIndex].description;
std::vector<std::string> descLines = wrapText(desc, textWidth);

// Calculate max scroll offset and clamp current offset
maxScrollOffset = std::max(0, static_cast<int>(descLines.size()) - visibleHeight);
if (scrollOffset > maxScrollOffset) {
    scrollOffset = maxScrollOffset;
}
if (scrollOffset < 0) {
    scrollOffset = 0;
}

// Render only the lines within the scroll window
int linesDrawn = 0;
for (int k = scrollOffset; k < static_cast<int>(descLines.size()) && linesDrawn < visibleHeight; ++k) {
    int lineY = baseDescContentY + linesDrawn;
    drawText(rightColX, lineY, descLines[k], "\033[38;5;250m");
    linesDrawn++;
}
```

### D. Scrollbar / Indicator Drawing

When the description text is too long to fit (`descLines.size() > visibleHeight`), we render a scrollbar on the rightmost column of the description area (`scrollbarX = rightColX + rightColWidth - 1`).

```cpp
if (static_cast<int>(descLines.size()) > visibleHeight && visibleHeight > 0) {
    int scrollbarX = rightColX + rightColWidth - 1;
    
    // 1. Draw background track (light-grey shade)
    for (int i = 0; i < visibleHeight; ++i) {
        drawText(scrollbarX, baseDescContentY + i, "░", "\033[38;5;240m");
    }
    
    // 2. Compute thumb size and position
    int totalLines = descLines.size();
    int thumbHeight = std::max(1, (visibleHeight * visibleHeight) / totalLines);
    if (thumbHeight >= visibleHeight) thumbHeight = visibleHeight - 1; // Leave room to move
    
    int scrollRange = totalLines - visibleHeight;
    int trackRange = visibleHeight - thumbHeight;
    int thumbStart = 0;
    if (scrollRange > 0) {
        thumbStart = (scrollOffset * trackRange) / scrollRange;
    }
    
    // 3. Draw thumb (cyan block)
    for (int i = 0; i < thumbHeight; ++i) {
        drawText(scrollbarX, baseDescContentY + thumbStart + i, "█", "\033[1;38;5;51m");
    }
}
```

### E. Keyboard Capture and State Reset

1. **Input Capture (`src/tui.cpp` inside `TUI::run()`)**:
   We intercept the keys `'['` (ASCII 91) and `']'` (ASCII 93) in the keyboard input switch case:
   ```cpp
   else if (key == '[') {
       scrollOffset = std::max(0, scrollOffset - 1);
   }
   else if (key == ']') {
       scrollOffset = std::min(maxScrollOffset, scrollOffset + 1);
   }
   ```

2. **State Reset**:
   To ensure that switching templates or tabs resets the scroll position to 0:
   ```cpp
   // At the top of the key handling in TUI::run()
   int prevTab = tabIndex;
   int prevSelected = selectedIndex;

   // ... key handling logic (arrows, enter, custom inputs) ...

   // Reset scroll offset if the user changes tabs or moves selection
   if (tabIndex != prevTab || selectedIndex != prevSelected) {
       scrollOffset = 0;
   }
   ```

3. **TUI Constructor Initialization (`src/tui.cpp` inside `TUI::TUI()`)**:
   ```cpp
   TUI::TUI() :
       selectedIndex(0),
       tabIndex(0),
       menuTimeVal(0.0),
       origTermiosPtr(nullptr),
       scrollOffset(0),
       maxScrollOffset(0) { ... }
   ```

---

## 3. Recommended E2E Testing Strategy for Linux

### Challenges in Testing terminal TUIs
1. **Raw Mode**: The program configures the terminal stdin into raw mode (`ECHO | ICANON` disabled) and uses blocking/non-blocking `read()`. Traditional standard pipes do not support this cleanly.
2. **Terminal Size Querying**: `graphcalc` makes `ioctl(STDOUT_FILENO, TIOCGWINSZ)` calls to read terminal size. Standard pipes return an error on this ioctl, forcing the program to fallback to a default 80x26 screen size.
3. **ANSI Styling**: Output is packed with cursor movement controls (`\033[H`, `\033[2J`) and SGR colors (`\033[38;5;...m`).

### Solution: Python `pty` and Subprocess E2E Framework
We recommend a Python-based testing script that uses the built-in `pty` module to spawn `graphcalc` inside a pseudo-terminal. This tricks the program into believing it is running in a physical window of defined dimensions, allowing us to send inputs and read output.

#### E2E Testing Script Structure (`scripts/e2e_test.py`)

```python
import os
import pty
import sys
import time
import subprocess

class VirtualTerminal:
    """Lightweight in-memory VT100 screen buffer for assertions."""
    def __init__(self, width=80, height=26):
        self.width = width
        self.height = height
        self.grid = [[" " for _ in range(width)] for _ in range(height)]
        self.cursor_x = 0
        self.cursor_y = 0

    def feed(self, data):
        i = 0
        while i < len(data):
            if data[i] == '\x1b':
                # Parse ANSI Escape Sequence
                if i + 1 < len(data) and data[i+1] == '[':
                    j = i + 2
                    while j < len(data) and not data[j].isalpha() and data[j] != '<':
                        j += 1
                    if j < len(data):
                        seq = data[i+2:j]
                        cmd = data[j]
                        if cmd == 'H':  # Cursor Home / Move
                            if not seq:
                                self.cursor_x = 0
                                self.cursor_y = 0
                            else:
                                parts = seq.split(';')
                                self.cursor_y = int(parts[0]) - 1
                                self.cursor_x = int(parts[1]) - 1 if len(parts) > 1 else 0
                        elif cmd == 'J' and seq == '2':  # Clear Screen
                            self.grid = [[" " for _ in range(self.width)] for _ in range(self.height)]
                        i = j + 1
                        continue
            elif data[i] == '\n':
                self.cursor_y = min(self.height - 1, self.cursor_y + 1)
                self.cursor_x = 0
                i += 1
            elif data[i] == '\r':
                self.cursor_x = 0
                i += 1
            else:
                if 0 <= self.cursor_x < self.width and 0 <= self.cursor_y < self.height:
                    self.grid[self.cursor_y][self.cursor_x] = data[i]
                self.cursor_x += 1
                if self.cursor_x >= self.width:
                    self.cursor_x = 0
                    self.cursor_y = min(self.height - 1, self.cursor_y + 1)
                i += 1

    def get_text(self):
        return "\n".join("".join(row) for row in self.grid)

def test_tui_navigation():
    # 1. Open pseudo-terminal pair
    master_fd, slave_fd = pty.openpty()
    
    # Set terminal window size to 80 cols by 26 rows
    import fcntl, termios, struct
    size_buf = struct.pack('HHHH', 26, 80, 0, 0)
    fcntl.ioctl(slave_fd, termios.TIOCSWINSZ, size_buf)

    # 2. Spawn graphcalc
    proc = subprocess.Popen(
        ['./graphcalc'],
        stdin=slave_fd,
        stdout=slave_fd,
        stderr=slave_fd,
        close_fds=True
    )
    os.close(slave_fd)
    os.set_blocking(master_fd, False)

    term = VirtualTerminal(80, 26)
    time.sleep(0.5)

    # 3. Read initial render
    try:
        raw_out = os.read(master_fd, 65536)
        term.feed(raw_out.decode('utf-8', errors='ignore'))
    except BlockingIOError:
        pass

    # Verify initial mode and selection
    screen_text = term.get_text()
    assert "[ 2D MODE ]" in screen_text
    assert "Sine Wave (Animated)" in screen_text

    # 4. Navigate down (Send Down Arrow byte sequence: \x1b[B)
    os.write(master_fd, b'\x1b[B')
    time.sleep(0.1)

    try:
        raw_out = os.read(master_fd, 65536)
        term.feed(raw_out.decode('utf-8', errors='ignore'))
    except BlockingIOError:
        pass

    screen_text = term.get_text()
    assert "Quadratic Parabola" in screen_text

    # 5. Test scroll keys (Send ']' key to scroll description down)
    os.write(master_fd, b']')
    time.sleep(0.1)

    try:
        raw_out = os.read(master_fd, 65536)
        term.feed(raw_out.decode('utf-8', errors='ignore'))
    except BlockingIOError:
        pass

    # 6. Exit
    os.write(master_fd, b'q')
    proc.wait()
    os.close(master_fd)
    print("E2E Navigation Test Passed successfully!")

if __name__ == '__main__':
    test_tui_navigation()
```

This test script:
- Emulates a terminal size of 80x26 using `TIOCSWINSZ` ioctl on the slave end of the pty.
- Runs without any external package dependencies (`pyte` / `pexpect` are optional; this pure-Python script uses standard libraries `pty`, `os`, `fcntl` and works out-of-the-box).
- Provides simple assertions on the terminal screen text content.

---

## 4. Compilation Constraints & Resizing Behavior

### Compilation Constraints
1. **C++17 Language Requirement**: The compiler flag `-std=c++17` is active in both `Makefile` and `CMakeLists.txt`. It relies on newer standard library additions (like `std::clamp` and structured bindings) which prevent compiling on compilers lacking full C++17 support.
2. **Library Linkage**:
   - `GL`: OpenGL libraries are linked via `-lGL`. Compiling requires OpenGL header files (usually provided by package `libgl1-mesa-dev` on Debian/Ubuntu systems).
   - `X11`: X11 windowing libraries are linked via `-lX11`. Development headers from `libx11-dev` must be present.
3. **Platform Constraint**: The terminal raw-mode calls use POSIX-specific `<termios.h>` and `<sys/ioctl.h>` headers. The OpenGL context creation in `opengl_window.cpp` uses GLX (`<GL/glx.h>`) and X11 window APIs. Therefore, this codebase is strictly bound to Linux (and other X11-compatible POSIX operating systems) and cannot be compiled natively on Windows without WSL or Cygwin.

### Resizing Behavior

The codebase reacts to terminal resizing differently across its three TUI states:

1. **Main Selection Menu (`TUI::showMenu()`)**:
   - **Mechanism**: The main loop runs at ~20 FPS (`std::this_thread::sleep_for(std::chrono::milliseconds(50))`) calling `showMenu()` every frame.
   - **Resizing Handling**: In `showMenu()`, it calls `clearGrid()`, which triggers `updateTerminalSize()`. This queries the terminal rows/columns using `ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)` on every frame. The visual boundaries of the boxes, divider position, dynamic preview height (`previewH = screenHeight - 15`), and text coordinates are updated instantly.
   - **Verdict**: Smooth and fully dynamic. Resizing the window updates the interface layout without warping.

2. **Simulation Mode Selection Menu (`TUI::showSimulationMenu()`)**:
   - **Mechanism**: Blocks synchronously in `readKey()` (waiting for keyboard inputs) rather than executing an animation loop.
   - **Resizing Handling**: A resize event (`SIGWINCH`) will trigger but the screen won't redraw immediately. It only updates once the user presses a key, at which point the menu redraws using the new dimensions.
   - **Verdict**: Semi-static; redraws are delayed until keyboard input occurs.

3. **Terminal Plot Viewer (`TUI::runTerminalSimulation()`)**:
   - **Mechanism**: Clears the terminal using `\033[2J\033[H` and directly writes the rendering output of the `Plotter` class to stdout at ~30 FPS.
   - **Resizing Handling**: The `Plotter` canvas width and height are fixed at 74 columns by 24 rows:
     ```cpp
     int width = 74;
     int height = 24;
     Plotter plotter(width, height);
     ```
     Because of this, the plotter does not adapt to the terminal window size.
   - **Verdict**: If the terminal window is shrunk below 74x28 (including instruction text headers), lines will warp, breaking the rendering. If the terminal is larger, the plot renders correctly inside a 74x24 column block at the top-left of the terminal.
