# GraphCalc TUI E2E Test Infrastructure

This document describes the testing infrastructure, design methodology, feature inventory, test runner usage, and test coverage mapping for the GraphCalc TUI application.

---

## 1. Test Runner Architecture

The GraphCalc E2E test runner (`tests/e2e_test.py`) is an opaque-box testing framework built entirely using Python's standard library. It simulates a user interacting with the TUI without modifying the C++ codebase, verifying behavior solely by sending input characters/sequences and inspecting the terminal's screen output.

### Key Components

1. **Pseudo-Terminal (pty) Controller (`TUIController`)**:
   - Spawns the `./graphcalc` process in a POSIX pseudo-terminal (`pty.openpty()`).
   - Configures a fixed terminal window size (default `80x26`) using `fcntl.ioctl` with `termios.TIOCSWINSZ` to ensure consistent rendering layouts.
   - Non-blocking read setup keeps execution robust, preventing hanging.
   - Sends keystrokes as raw byte streams (e.g. arrow keys `\x1b[A`, scroll keys `[`, `]`, custom mode triggers, enter keys).
   
2. **ANSI Screen Buffer (`ScreenBuffer`)**:
   - Parses incoming ANSI escape sequences (CSI sequences like Cursor Position `\x1b[row;colH`, Erase Screen `\x1b[2J`, Clear Line `\x1b[K`, and color transitions `\x1b[...m`).
   - Maintains an in-memory character grid (`grid[rows][cols]`) and matching color map (`colors[rows][cols]`) reproducing what a terminal emulator displays.

3. **Text Extractors**:
   - Extracts complete screen dumps for general UI assertions.
   - Extracts specific text lines corresponding to the right-panel description box (taking into account the screen width/height layout logic).

---

## 2. Feature Inventory

The test suite validates three main functional categories:
1. **Descriptions**: Displaying contextual template details on the right panel.
2. **Scrolling**: Scrolling through long descriptions using `[` and `]`.
3. **Scroll-Reset**: Resetting the description scroll position to `0` upon changing selection item or switching mode tabs.

---

## 3. Test Coverage & Mapping (38 Test Cases)

### Tier 1: Feature Coverage (15 Tests)
Happy path validations verifying navigation, mode switching, description display, and basic scroll keys.

- `test_tab_navigation_2d`: Checks default 2D Sine Wave description loads.
- `test_tab_navigation_3d`: Tab right to 3D, checks Ripple Waves description loads.
- `test_tab_navigation_vis`: Tab right twice to Visuals, checks Mandelbrot description loads.
- `test_navigation_down_2d`: Select Quadratic Parabola in 2D, checks description.
- `test_navigation_down_3d`: Select Saddle in 3D, checks description.
- `test_navigation_down_vis`: Select Julia Set in Visuals, checks description.
- `test_description_content_dampened_wave`: Dampened Wave description contains decay details.
- `test_description_content_sombrero`: Sombrero description contains sombrero details.
- `test_description_content_plasma`: Organic Plasma description contains organic/interference details.
- `test_scrolling_key_down_ignored_if_no_scroll`: Down scroll key on short description does not modify view.
- `test_scrolling_key_up_ignored_at_top`: Up scroll key at scroll offset 0 does not modify view.
- `test_scroll_down_action`: Down scroll key on long description shifts lines (expected to fail on unmodified code).
- `test_scroll_up_action`: Down then up scroll key on long description restores top lines (expected to fail on unmodified code).
- `test_scroll_reset_on_next_item`: Scrolling down, changing selection, ensures scroll offset is reset to 0.
- `test_scroll_reset_on_tab_change`: Scrolling down, switching tab, ensures scroll offset is reset to 0.

### Tier 2: Boundary & Corner Cases (15 Tests)
Borders, limits, window sizes, and boundary limits.

- `test_short_description_no_scroll`: MaxScroll limit should be 0 for short descriptions.
- `test_long_description_wrapping`: Text wrapping behavior for descriptions exceeding the right panel width.
- `test_scroll_at_upper_boundary`: Clamping of scroll offset to `>= 0` on multiple scroll ups.
- `test_scroll_at_lower_boundary`: Clamping of scroll offset to `maxScroll` on multiple scroll downs (expected to fail on unmodified code).
- `test_terminal_resize_small_width`: Layout divider shifts to col 30 when window width is 60.
- `test_terminal_resize_large_width`: Layout divider remains at col 42 when window width is 100.
- `test_terminal_resize_small_height`: Minimum safe height rendering (15 lines) works without crashing.
- `test_terminal_resize_large_height`: Height scales to 40 lines successfully, increasing description area.
- `test_scroll_reset_at_boundary`: Scrolling to bottom of long description, selecting another template, and checking it reset.
- `test_scroll_keys_invalid_in_simulation`: Scroll keys do nothing when inside simulation view mode selection.
- `test_scroll_keys_in_custom_input`: Scroll keys are parsed as text characters (or ignored) inside custom formula prompt instead of scrolling.
- `test_scroll_up_down_repeated`: Stress testing repeated scroll key combinations to ensure screen buffer integrity.
- `test_navigation_out_of_bounds_down`: Moving down past the end of the template list clamps to the last item.
- `test_navigation_out_of_bounds_up`: Moving up past the first template clamps to the first item.
- `test_empty_description_handling`: Safety fallback/handling of empty/very short boundaries.

### Tier 3: Cross-Feature Combinations (3 Tests)
Intertwining workflows (scrolling, input modes, menu switching).

- `test_scroll_navigate_reset_cycle`: Scroll down -> select next -> select back -> verify original is reset to 0.
- `test_custom_input_and_description`: Open custom prompt -> input equation -> go to simulation menu -> exit back to menu -> check description integrity.
- `test_tab_switch_selection_reset`: Go down -> switch tab -> verify selection resets to 0 (Ripple Waves) on new tab.

### Tier 4: Real-World Application (5 Tests)
Simulating continuous sequences of a human user exploring the application.

- `test_user_scenario_2d_read`: Selecting Dampened Wave, scrolling description, entering simplified terminal simulation, verifying active simulation status.
- `test_user_scenario_3d_read`: Tab to 3D, select Sombrero, scroll description, launch terminal simulation.
- `test_user_scenario_vis_read`: Tab to Visuals, select Julia Set, scroll, launch simulation.
- `test_user_scenario_custom_flow`: Launch custom equation input, type `x^2`, launch simulation, quit back to simulation menu, quit back to main menu.
- `test_user_scenario_comprehensive`: Swap tabs, navigate selections, scroll, resize width, exit cleanly.

---

## 4. How to Run the Tests

To run the full E2E test suite:

```bash
python3 tests/e2e_test.py
```

The test runner will compile the `graphcalc` binary if it does not already exist, execute all 38 tests, print an ANSI-colored status report for each test case, and exit with an error code equal to the number of failed tests.
