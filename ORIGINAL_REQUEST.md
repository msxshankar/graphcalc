# Original User Request

## Initial Request — 2026-06-06T22:51:09Z

The project involves expanding the description of every 2D, 3D, and visualization template in the `graphcalc` codebase to include the relevant mathematics and history behind it, and adding scrolling capabilities for these descriptions in the TUI menu.

Working directory: /home/mayur/Documents/repos/graphcalc
Integrity mode: development

## Requirements

### R1. Mathematical & Historical Descriptions
- Expand all descriptions in `templates2D`, `templates3D`, and `templatesVis` in `src/tui.cpp` (and any other menu files) into natural, flowing paragraphs.
- Descriptions should explain the mathematics and history of the curve, surface, or visualization, written for a general audience that has some math knowledge.

### R2. Description Scrolling in TUI
- Implement vertical scrolling for the description box in the TUI menu.
- If a description is too long to fit in the available vertical space below the preview box, the TUI should display a scrollbar or indicator and allow the user to scroll up and down using the `[` and `]` keys.
- Changing the selected template or switching tabs should reset the scroll offset back to 0.

## Acceptance Criteria

### Content Quality
- [ ] Every template has a description containing at least 80 characters of detailed math and history explanation.
- [ ] Explanations are written as natural-flowing prose, not bullet points or simple definitions.

### Scrolling & Layout
- [ ] Pressing `[` scrolls the current description up, and `]` scrolls it down.
- [ ] Description text wraps correctly inside the right-column boundary without overflowing.
- [ ] Switching between tabs or templates resets the description scroll position to the top.
- [ ] The codebase compiles cleanly with `make clean && make`.
