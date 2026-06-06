## 2026-06-06T23:53:21Z

You are the E2E Testing Worker. Your working directory is /home/mayur/Documents/repos/graphcalc/.agents/e2e_testing_track/.

Your task is to design, write, and execute a comprehensive, opaque-box E2E test suite for the graphcalc TUI description and scrolling features.

MANDATORY INTEGRITY WARNING:
DO NOT CHEAT. All implementations must be genuine. DO NOT hardcode test results, create dummy/facade implementations, or circumvent the intended task. A Forensic Auditor will independently verify your work. Integrity violations WILL be detected and your work WILL be rejected.

Scope & Methodology:
- Use Category-Partition, Boundary Value Analysis, Pairwise, and Workload Testing.
- Implement the test suite in Python under `tests/e2e_test.py` (create `tests/` directory at the project root).
- Implement a pseudo-terminal (pty) controller that launches `./graphcalc`, sets a specific terminal window size (e.g. 80x26), sends raw keystrokes (like Down Arrow, '[', ']', Right Arrow, etc.), and parses the rendered ANSI screen grid in-memory.
- Do not depend on external Python packages; use standard libraries (`os`, `sys`, `pty`, `fcntl`, `termios`, `subprocess`, `time`).

Requirements for the test cases (minimum thresholds, N = 3 features: Descriptions, Scrolling, Scroll-Reset):
- Tier 1 (Feature Coverage): >= 15 test cases (happy path testing, e.g. navigating to different templates and tabs, verifying descriptions display).
- Tier 2 (Boundary & Corner Cases): >= 15 test cases (e.g. short/very long descriptions, scrolling at top boundary [0], scrolling at bottom boundary [maxScroll], resizing terminal to various small/large bounds, etc.).
- Tier 3 (Cross-Feature Combinations): >= 3 test cases (e.g. scrolling, then navigating selection, then checking scroll reset; custom input simulation combined with description check).
- Tier 4 (Real-World Application): >= 5 test cases (e.g. typical user scenarios of selecting 2D, 3D, and Vis templates, reading their full descriptions by scrolling through them completely).
- Total test cases: At least 38 test cases.

Deliverables:
1. `tests/e2e_test.py` containing the python test suite. It must support running all tests and print a clean summary.
2. `TEST_INFRA.md` in your working directory (`/home/mayur/Documents/repos/graphcalc/.agents/e2e_testing_track/TEST_INFRA.md`) detailing the feature inventory, test runner usage, and coverage.
3. `TEST_READY.md` in the project root (`/home/mayur/Documents/repos/graphcalc/TEST_READY.md`) indicating that the test suite is ready, with the exact test runner commands and a checklist.

Run the test suite on the current (unmodified) codebase, document the failures (since descriptions aren't expanded and scrolling isn't implemented yet, most tests will fail or skip; handle this gracefully), and output the result in your handoff report. Write your handoff report to `handoff.md` in your working directory. Let me know when complete.
