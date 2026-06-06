## 2026-06-06T22:51:59Z
Examine the graphcalc codebase, specifically src/tui.cpp, src/tui.hpp, and related files.
Write a detailed report containing:
1. Complete mathematical and historical descriptions (at least 80 characters of detailed natural prose) for every single one of the 30 templates (11 2D templates, 11 3D templates, 8 Vis templates).
2. A detailed implementation plan for vertical scrolling in the TUI description box using '[' and ']' keys (resetting scroll to 0 on template/tab change), including:
   - Necessary member variables in TUI class (e.g. scrollOffset, maxScrollOffset).
   - How to compute the available vertical height for the description box (visibleHeight) dynamically depending on terminal size and whether the formula is displayed.
   - How to wrap the description text correctly and render only the visible lines based on scrollOffset.
   - How to draw a scrollbar or scroll indicator (e.g. showing scroll tracks and thumb or arrows) when text is too long to fit.
   - How/where to capture '[' and ']' key inputs and handle them.
3. Recommendations for an E2E testing strategy/framework that runs on Linux (e.g. a Python script using subprocess to run graphcalc, feeding it key sequences, and validating screen output or screen_* files).
4. Verify compiling constraints and how graphcalc behaves under terminal resizing.

Write your findings to a file named 'analysis.md' in your working directory. Let me know once done.
