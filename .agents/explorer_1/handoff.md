# Handoff Report — explorer_1

## 1. Observation
- **Template definitions**: In `src/tui.cpp` (lines 49-89), 30 templates are defined static-inline inside the `TUI` constructor: `templates2D` has 11 templates, `templates3D` has 11 templates, and `templatesVis` has 8 templates.
- **Description rendering**: In `src/tui.cpp` (lines 764-781), descriptions are wrapped to `rightColWidth` columns and drawn in a loop starting at row `descY + 1`. It breaks drawing if the row index `lineY >= screenHeight - 3`.
- **Keyboard inputs**: In `src/tui.cpp` (lines 638-688), keyboard inputs are read via `readKey(true)` inside a 20 FPS loop.
- **Terminal size queries**: In `src/tui.cpp` (lines 95-107), `updateTerminalSize()` fetches window dimensions via `ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)`.
- **Compilation verification**: Running `make clean && make` successfully builds `graphcalc` with `g++ -std=c++17` linking `-lX11 -lGL`.
- **Reference screens**: Files like `screen_2d_0.txt` in the root folder show raw terminal output with ANSI sequences including SGR colors and cursor positions.

## 2. Logic Chain
- **Word Wrapping**: The current character-slicing logic (`desc.substr(pos, maxLen)`) splits words in half at line boundaries. A word-boundary-aware wrapper will improve layout cleanliness.
- **Description Box height**: The description box overlaps with the formula if `lineY` reaches the formula coordinates. Dynamically calculating `visibleHeight = limitY - baseDescContentY + 1` (where `limitY` is `screenHeight - 5` if the formula fits, or `screenHeight - 2` otherwise) guarantees no overlapping layout bugs under resize events.
- **Input for scrolling**: The characters `'['` and `']'` correspond to raw ASCII 91 and 93, which are read by `readKey(true)` and can be handled directly as conditional branches inside `run()` to increment/decrement `scrollOffset` within `[0, maxScrollOffset]`.
- **State reset**: Resetting `scrollOffset = 0` whenever `selectedIndex != prevSelected || tabIndex != prevTab` ensures the viewport is at the beginning when navigating.
- **E2E testing**: Testing raw mode termios and terminal sizes require a virtual terminal. Spawning the subprocess under `pty` in Python and tracking the screen grid with a basic parser allows verification of the TUI screens without requiring manual graphical interaction.

## 3. Caveats
- The OpenGL window mode requires a working X11 display context (`$DISPLAY`), which means automated E2E tests executing the OpenGL window option will fail in headless environments unless wrapped inside a virtual framebuffer server like `xvfb-run`.
- The terminal simulation mode utilizes a fixed 74x24 plotter grid, meaning it will warp if the terminal is resized below 74 columns or 28 rows, unlike the main menu which scales dynamically.

## 4. Conclusion
- The mathematical and historical expansions for all 30 templates have been drafted in `analysis.md`.
- A fully functional scrolling plan utilizing dynamic viewport sizing and word wrapping has been formulated.
- An E2E Python framework using virtual terminals has been designed to validate screen state captures and key sequences.

## 5. Verification Method
1. To verify compilation, run `make clean && make`.
2. Inspect the detailed report `/home/mayur/Documents/repos/graphcalc/.agents/explorer_1/analysis.md` containing all descriptions and implementation steps.
3. Once the scroll implementation is completed by the implementer, run the E2E script via `python3 scripts/e2e_test.py` to confirm scrolling and menu behavior.
