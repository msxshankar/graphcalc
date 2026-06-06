## 2026-06-06T22:59:31Z
You are the TUI scrolling and description implementation Worker. Your working directory is /home/mayur/Documents/repos/graphcalc/.agents/worker_impl_1/.

Your task is to implement the expanded mathematical/historical descriptions and description box scrolling in graphcalc.

MANDATORY INTEGRITY WARNING:
DO NOT CHEAT. All implementations must be genuine. DO NOT hardcode test results, create dummy/facade implementations, or circumvent the intended task. A Forensic Auditor will independently verify your work. Integrity violations WILL be detected and your work WILL be rejected.

Detailed Instructions:
1. Mathematical & Historical Descriptions:
   - Read the 30 detailed descriptions (each >= 80 characters of natural prose explaining math and history) from '/home/mayur/Documents/repos/graphcalc/.agents/explorer_1/analysis.md'.
   - Update `src/tui.cpp` to initialize `templates2D`, `templates3D`, and `templatesVis` in the `TUI::TUI()` constructor with these exact detailed descriptions. Ensure NO description is abbreviated or simplified.
   
2. TUI Description Box Scrolling:
   - In `src/tui.hpp`, add member variables `int scrollOffset;` and `int maxScrollOffset;` to the `TUI` class.
   - In `src/tui.cpp`, in the `TUI::TUI()` constructor, initialize `scrollOffset(0)` and `maxScrollOffset(0)`.
   - In `TUI::run()`, capture key presses for '[' (ASCII 91) and ']' (ASCII 93).
     - If '[' is pressed, decrement `scrollOffset`, clamping it at >= 0.
     - If ']' is pressed, increment `scrollOffset`, clamping it at <= `maxScrollOffset`.
   - Reset `scrollOffset = 0` whenever `selectedIndex` or `tabIndex` changes. (Tip: track `prevSelected` and `prevTab` before key processing in `TUI::run()`'s loop, and if they change, reset `scrollOffset = 0`).
   - In `TUI::showMenu()`, calculate the available vertical space for the description content dynamically:
     - `int descY = 4 + previewH + 1;`
     - `int baseDescContentY = descY + 1;`
     - Check if formula is displayed: `bool showFormula = (screenHeight - 3 > 4 + previewH + 3);`
     - `int limitY = showFormula ? (screenHeight - 5) : (screenHeight - 2);`
     - `int visibleHeight = limitY - baseDescContentY + 1;`
     - If `visibleHeight < 0` set it to 0.
   - Implement word wrapping for the description text using a helper function. Wrap the text at `maxLen = rightColWidth - 2` columns.
   - Draw only the subset of wrapped description lines corresponding to the viewport `[scrollOffset, scrollOffset + visibleHeight)`.
   - Draw a scrollbar on the rightmost column of the description area (`scrollbarX = rightColX + rightColWidth - 1`) if `descLines.size() > visibleHeight`:
     - Draw background track characters `░` from `baseDescContentY` to `baseDescContentY + visibleHeight - 1`.
     - Calculate thumb height: `thumbHeight = max(1, (visibleHeight * visibleHeight) / descLines.size())`. Ensure it is less than `visibleHeight`.
     - Calculate thumb start: `thumbStart = (scrollOffset * (visibleHeight - thumbHeight)) / (descLines.size() - visibleHeight)`.
     - Draw thumb characters `█` (using color `\033[1;38;5;51m`) for `thumbHeight` rows.

3. Compilation and E2E verification:
   - Run `make clean && make` to build. Ensure no compilation errors.
   - Run E2E test suite: `python3 tests/e2e_test.py`. Verify that all 38 tests pass 100%!
   - If any test fails, inspect the failure, adjust the implementation, and re-run.

Write your handoff report to `handoff.md` in your working directory. Include compile commands, compilation stdout/stderr, and E2E test runner outputs. Let me know when complete.
