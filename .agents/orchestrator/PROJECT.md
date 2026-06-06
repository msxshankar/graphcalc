# Project: graphcalc Description Expansion and TUI Scrolling

## Architecture
- `src/tui.hpp` and `src/tui.cpp`: Main Terminal User Interface.
- `src/main.cpp`: Entry point.
- `src/parser.cpp` / `src/parser.hpp`: Equation parser.
- `src/plotter.cpp` / `src/plotter.hpp`: Grid-based ASCII plotting engine.

The data flow for descriptions in the TUI is:
1. `TUI::TUI()` initializes template vectors (`templates2D`, `templates3D`, `templatesVis`) with descriptions.
2. `TUI::showMenu()` retrieves the description for the currently selected template and tab.
3. `TUI::showMenu()` wraps the description text based on available right-column width.
4. `TUI::showMenu()` displays the description text. With scrolling, a scrollbar indicator is rendered, and the text lines drawn are offset by `scrollOffset`.

## Code Layout
- `src/tui.hpp`: Define scroll properties (`scrollOffset` and potentially scrolling helper methods).
- `src/tui.cpp`: Update template initializations, handle `[` and `]` keypresses, manage scrollbar drawing and offset rendering in `showMenu()`.

## Milestones
| # | Name | Scope | Dependencies | Status |
|---|------|-------|-------------|--------|
| 1 | Math & Historical Descriptions | Expand all 2D, 3D, and Vis template descriptions to >= 80 chars of math & history prose. | None | PLANNED |
| 2 | TUI Description Scrolling | Implement vertical scrolling via '[' and ']', scrollbar rendering, text wrapping, and scroll resetting on change. | M1 | PLANNED |
| 3 | E2E Integration & Verification | Pass E2E test suite (Dual Track), run adversarial testing. | M1, M2 | PLANNED |

## Interface Contracts
### TUI ↔ Keyboard Input
- Key `'['` (ASCII 91): Scroll description UP.
- Key `']'` (ASCII 93): Scroll description DOWN.
- Key navigation (Arrow keys, tab switching): Reset scroll position to 0.
