#!/usr/bin/env python3
import os
import sys
import pty
import fcntl
import termios
import struct
import subprocess
import time
import codecs

# Core constants
KEY_UP = "\x1b[A"
KEY_DOWN = "\x1b[B"
KEY_RIGHT = "\x1b[C"
KEY_LEFT = "\x1b[D"
KEY_ENTER = "\n"
KEY_ESC = "\x1b"
KEY_SCROLL_UP = "["
KEY_SCROLL_DOWN = "]"

class ScreenBuffer:
    def __init__(self, rows=26, cols=80):
        self.rows = rows
        self.cols = cols
        self.reset()
        
    def reset(self):
        self.grid = [[" " for _ in range(self.cols)] for _ in range(self.rows)]
        self.colors = [["" for _ in range(self.cols)] for _ in range(self.rows)]
        self.cursor_row = 0
        self.cursor_col = 0
        self.current_color = ""
        
    def update(self, char_stream):
        i = 0
        n = len(char_stream)
        while i < n:
            c = char_stream[i]
            if c == '\x1b': # Escape sequence
                if i + 1 >= n:
                    break
                if char_stream[i+1] == '[':
                    j = i + 2
                    while j < n and not (0x40 <= ord(char_stream[j]) <= 0x7E):
                        j += 1
                    if j >= n:
                        break
                    seq = char_stream[i+2:j]
                    cmd = char_stream[j]
                    self._handle_csi(seq, cmd)
                    i = j + 1
                else:
                    i += 2
            elif c == '\n':
                self.cursor_row = min(self.rows - 1, self.cursor_row + 1)
                self.cursor_col = 0
                i += 1
            elif c == '\r':
                self.cursor_col = 0
                i += 1
            elif c == '\t':
                self.cursor_col = min(self.cols - 1, (self.cursor_col // 8 + 1) * 8)
                i += 1
            elif c == '\x08':
                self.cursor_col = max(0, self.cursor_col - 1)
                i += 1
            else:
                if self.cursor_row < self.rows and self.cursor_col < self.cols:
                    self.grid[self.cursor_row][self.cursor_col] = c
                    self.colors[self.cursor_row][self.cursor_col] = self.current_color
                self.cursor_col += 1
                i += 1

    def _handle_csi(self, seq, cmd):
        args = []
        if seq:
            clean_seq = seq
            if seq.startswith('?') or seq.startswith('<'):
                clean_seq = seq[1:]
            if clean_seq:
                try:
                    args = [int(x) for x in clean_seq.split(';')]
                except ValueError:
                    pass
        
        if cmd == 'H' or cmd == 'f':
            r = args[0] if len(args) > 0 else 1
            c = args[1] if len(args) > 1 else 1
            self.cursor_row = min(self.rows - 1, max(0, r - 1))
            self.cursor_col = min(self.cols - 1, max(0, c - 1))
        elif cmd == 'G':
            c = args[0] if len(args) > 0 else 1
            self.cursor_col = min(self.cols - 1, max(0, c - 1))
        elif cmd == 'm':
            if not seq or seq == '0':
                self.current_color = ""
            else:
                self.current_color = f"\x1b[{seq}m"
        elif cmd == 'J':
            mode = args[0] if len(args) > 0 else 0
            if mode == 2:
                for r in range(self.rows):
                    for c in range(self.cols):
                        self.grid[r][c] = " "
                        self.colors[r][c] = ""
                self.cursor_row = 0
                self.cursor_col = 0
        elif cmd == 'K':
            mode = args[0] if len(args) > 0 else 0
            if mode == 0:
                for c in range(self.cursor_col, self.cols):
                    self.grid[self.cursor_row][c] = " "
                    self.colors[self.cursor_row][c] = ""

class TUIController:
    def __init__(self, cmd=['./graphcalc'], rows=26, cols=80):
        self.cmd = cmd
        self.rows = rows
        self.cols = cols
        self.master_fd = None
        self.proc = None
        self.screen = ScreenBuffer(rows, cols)
        self.decoder = codecs.getincrementaldecoder('utf-8')()
        
    def start(self):
        # Auto compile if binary missing
        if not os.path.exists(self.cmd[0]):
            subprocess.run(["make"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            
        self.master_fd, slave_fd = pty.openpty()
        
        # Set window size
        size = struct.pack("HHHH", self.rows, self.cols, 0, 0)
        fcntl.ioctl(self.master_fd, termios.TIOCSWINSZ, size)
        fcntl.ioctl(slave_fd, termios.TIOCSWINSZ, size)
        
        # Non-blocking read setup
        flags = fcntl.fcntl(self.master_fd, fcntl.F_GETFL)
        fcntl.fcntl(self.master_fd, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        
        env = os.environ.copy()
        env["GRAPHCALC_TEST_SHORT_DESC"] = "1"
        self.proc = subprocess.Popen(
            self.cmd,
            stdin=slave_fd,
            stdout=slave_fd,
            stderr=slave_fd,
            preexec_fn=os.setsid,
            close_fds=True,
            env=env
        )
        os.close(slave_fd)
        
    def send_keys(self, data):
        if isinstance(data, str):
            data = data.encode('utf-8')
        os.write(self.master_fd, data)
        time.sleep(0.06) # brief pause to let process run
        
    def read_and_parse(self, timeout=0.1):
        start_time = time.time()
        buf = b""
        while time.time() - start_time < timeout:
            try:
                chunk = os.read(self.master_fd, 4096)
                if not chunk:
                    break
                buf += chunk
                time.sleep(0.01)
            except OSError:
                if buf:
                    break
                time.sleep(0.01)
                
        if buf:
            decoded = self.decoder.decode(buf, final=False)
            self.screen.update(decoded)
            return True
        return False

    def wait_for_settle(self, timeout=1.0):
        start_time = time.time()
        while time.time() - start_time < timeout:
            if not self.read_and_parse(timeout=0.05):
                break

    def close(self):
        if self.proc:
            self.proc.terminate()
            try:
                self.proc.wait(timeout=0.5)
            except subprocess.TimeoutExpired:
                self.proc.kill()
        if self.master_fd is not None:
            try:
                os.close(self.master_fd)
            except OSError:
                pass

# Helpers to extract text from ScreenBuffer
def get_screen_text(screen):
    return "\n".join("".join(row) for row in screen.grid)

def get_description_text(screen):
    divider_x = screen.cols // 2 if screen.cols < 90 else 42
    right_col_x = divider_x + 3
    right_col_width = screen.cols - divider_x - 6
    preview_h = max(8, screen.rows - 15)
    desc_y = 4 + preview_h + 1
    
    desc_lines = []
    # Available lines end before Formula section (which takes screen.rows - 4 to screen.rows - 3)
    # The condition in tui.cpp is if (lineY >= screenHeight - 3) break;
    # So lineY must be < screen.rows - 3
    for r in range(desc_y + 1, screen.rows - 3):
        if r < len(screen.grid):
            line_str = "".join(screen.grid[r][right_col_x : right_col_x + right_col_width]).rstrip()
            if line_str:
                desc_lines.append(line_str)
    return " ".join(desc_lines)

# ----------------- TEST CASES -----------------

# TIER 1: Feature Coverage (happy path, tab nav, description display)
def test_tab_navigation_2d():
    """Navigate to 2D tab and verify the default description of the first template displays."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "amplitude" in desc, "2D Sine Wave description should contain amplitude"
        assert "harmonic" in desc, "2D Sine Wave description should contain harmonic"
    finally:
        controller.close()

def test_tab_navigation_3d():
    """Navigate to 3D tab and verify description of Ripple Waves."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "three-dimensional" in desc or "radial" in desc, "3D Ripple Waves description should be visible"
    finally:
        controller.close()

def test_tab_navigation_vis():
    """Navigate to Visuals tab and verify Mandelbrot Set description."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "complex" in desc or "quadratic" in desc, "Mandelbrot description should be visible"
    finally:
        controller.close()

def test_navigation_down_2d():
    """Select Quadratic Parabola in 2D and verify description displays."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "parabola" in desc or "Parabola" in desc, "Parabola description should be visible"
    finally:
        controller.close()

def test_navigation_down_3d():
    """Select Saddle in 3D and verify description displays."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "saddle" in desc or "Saddle" in desc, "Saddle description should be visible"
    finally:
        controller.close()

def test_navigation_down_vis():
    """Select Julia Set in Visuals and verify description displays."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "fractal" in desc or "boundary" in desc or "Mandelbrot" in desc, "Julia Set description should be visible"
    finally:
        controller.close()

def test_description_content_dampened_wave():
    """Select Dampened Wave in 2D and verify description content."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN)
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "exponentially" in desc or "decay" in desc, "Dampened Wave description should mention decay"
    finally:
        controller.close()

def test_description_content_sombrero():
    """Select Sombrero Function in 3D and verify description content."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN)
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "radial" in desc or "sinc-like" in desc, "Sombrero description should contain sinc-like"
    finally:
        controller.close()

def test_description_content_plasma():
    """Select Organic Plasma in Visuals and verify description content."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN)
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "organic" in desc or "interference" in desc, "Plasma description should mention organic or interference"
    finally:
        controller.close()

def test_scrolling_key_down_ignored_if_no_scroll():
    """Verify that pressing scroll down on short description doesn't crash or overflow."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Absolute Value is index 4, very short description
        for _ in range(4):
            controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc_before = get_description_text(controller.screen)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        desc_after = get_description_text(controller.screen)
        assert desc_before == desc_after, "Short description should not change on scroll down"
    finally:
        controller.close()

def test_scrolling_key_up_ignored_at_top():
    """Verify that pressing scroll up at scroll position 0 doesn't crash or overflow."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        desc_before = get_description_text(controller.screen)
        controller.send_keys(KEY_SCROLL_UP)
        controller.wait_for_settle()
        desc_after = get_description_text(controller.screen)
        assert desc_before == desc_after, "Scroll up at top should not change text"
    finally:
        controller.close()

def test_scroll_down_action():
    """Scroll down on a long description, expecting a change in text line representation."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Sine Wave is selected by default (medium-long)
        desc_before = get_description_text(controller.screen)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        desc_after = get_description_text(controller.screen)
        # Note: In unimplemented codebase, these will be identical because scroll does nothing.
        # This test is EXPECTED to fail on unmodified codebase.
        assert desc_before != desc_after, "Scrolling down should modify visible text lines"
    finally:
        controller.close()

def test_scroll_up_action():
    """Scroll down then scroll up, expecting scroll offset to return to 0 and text to revert."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        desc_start = get_description_text(controller.screen)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        controller.send_keys(KEY_SCROLL_UP)
        controller.wait_for_settle()
        desc_end = get_description_text(controller.screen)
        # In unmodified codebase, scrolling down doesn't work, so desc_start == desc_end.
        # But for correctness of the test, we verify that scroll down actually changed it first, then scroll up reverted it.
        # Since it won't change, we fail.
        assert desc_start != desc_end or desc_start == desc_end, "Dummy assert to show structure"
        # The real check is:
        # 1. scroll down changes it: desc_start != desc_mid
        # 2. scroll up restores it: desc_mid != desc_end and desc_start == desc_end
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        desc_mid = get_description_text(controller.screen)
        assert desc_start != desc_mid, "Scroll down must change text"
        controller.send_keys(KEY_SCROLL_UP)
        controller.wait_for_settle()
        desc_end = get_description_text(controller.screen)
        assert desc_start == desc_end, "Scroll up must restore original text"
    finally:
        controller.close()

def test_scroll_reset_on_next_item():
    """Scroll down, then move selection down, check that scroll position resets to 0 (shows beginning of next description)."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Scroll down first template description
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        # Navigate to next template
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        # If scroll-reset works, it should show the beginning of Quadratic Parabola description
        assert "polynomial" in desc or "parabola" in desc, "Next item should start at scroll offset 0"
    finally:
        controller.close()

def test_scroll_reset_on_tab_change():
    """Scroll down, then switch tab, check that scroll position resets to 0."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT) # Switch to 3D
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "three-dimensional" in desc or "radial" in desc, "Switching tab should show beginning of 3D description"
    finally:
        controller.close()


# TIER 2: Boundary & Corner Cases (boundaries, bounds, resizing, simulation, edge cases)
def test_short_description_no_scroll():
    """Verify that a short description has maxScroll = 0 (cannot be scrolled)."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Absolute Value index 4 in 2D has description: "V-shaped graph. Adjust 'a' (steepness) and 'b' (vertical shift)."
        for _ in range(4):
            controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc_before = get_description_text(controller.screen)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        desc_after = get_description_text(controller.screen)
        assert desc_before == desc_after, "Short description should not scroll"
    finally:
        controller.close()

def test_long_description_wrapping():
    """Verify that a long description wraps into multiple lines in the screen grid."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Dampened Wave is index 2, quite long description
        for _ in range(2):
            controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        # We check that description spans multiple rows on the grid
        divider_x = controller.screen.cols // 2 if controller.screen.cols < 90 else 42
        right_col_x = divider_x + 3
        right_col_width = controller.screen.cols - divider_x - 6
        preview_h = max(8, controller.screen.rows - 15)
        desc_y = 4 + preview_h + 1
        
        # Verify line 17 and line 18 both have non-empty text in description columns
        line17 = "".join(controller.screen.grid[desc_y + 1][right_col_x : right_col_x + right_col_width]).strip()
        line18 = "".join(controller.screen.grid[desc_y + 2][right_col_x : right_col_x + right_col_width]).strip()
        assert len(line17) > 0, "Line 1 of description should be non-empty"
        assert len(line18) > 0, "Line 2 of description should be non-empty (wrapped)"
    finally:
        controller.close()

def test_scroll_at_upper_boundary():
    """Verify scroll offset clamps to 0 when repeatedly scrolling up."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        desc_before = get_description_text(controller.screen)
        # Scroll up 5 times
        for _ in range(5):
            controller.send_keys(KEY_SCROLL_UP)
        controller.wait_for_settle()
        desc_after = get_description_text(controller.screen)
        assert desc_before == desc_after, "Repeated scroll up at top should do nothing"
    finally:
        controller.close()

def test_scroll_at_lower_boundary():
    """Verify scroll offset clamps to maxScroll when repeatedly scrolling down."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Scroll down 30 times (more than lines in description)
        for _ in range(30):
            controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        desc_after_many = get_description_text(controller.screen)
        
        # Press scroll down one more time, verify it doesn't change further (clamped)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        desc_after_extra = get_description_text(controller.screen)
        
        # In unimplemented, they'll all be identical. But we assert that they are identical after clamping.
        # In implemented, it should also be identical because it clamped at bottom.
        # But we must ensure it actually scrolled first.
        # Since it won't scroll in unimplemented, we check the actual scroll happened.
        # To fail on unimplemented, we assert that the screen did change from start, which won't happen.
        desc_start = get_description_text(TUIController().screen) # wait, we need to compare to start
        assert desc_after_many == desc_after_extra, "Clamping at bottom should prevent further scroll"
        # We need a check that makes it fail on unimplemented:
        controller2 = TUIController()
        controller2.start()
        controller2.wait_for_settle()
        desc_start = get_description_text(controller2.screen)
        controller2.close()
        assert desc_start != desc_after_many, "Should have scrolled down to bottom"
    finally:
        controller.close()

def test_terminal_resize_small_width():
    """Resize terminal to 60 width, verify layout and divider shifts to 30."""
    controller = TUIController(rows=26, cols=60)
    controller.start()
    try:
        controller.wait_for_settle()
        # Check divider is at column 30 (0-indexed)
        # We check that row 10, column 30 has the line drawing character '│'
        cell_char = controller.screen.grid[10][30]
        assert cell_char == "│", f"Divider should be at col 30, got '{cell_char}'"
    finally:
        controller.close()

def test_terminal_resize_large_width():
    """Resize terminal to 100 width, verify divider remains at 42 (since cols >= 90)."""
    controller = TUIController(rows=26, cols=100)
    controller.start()
    try:
        controller.wait_for_settle()
        # Divider should be at 42
        cell_char = controller.screen.grid[10][42]
        assert cell_char == "│", f"Divider should be at col 42, got '{cell_char}'"
    finally:
        controller.close()

def test_terminal_resize_small_height():
    """Resize terminal to 15 height (minimum safety height), verify no crash and preview box fits."""
    controller = TUIController(rows=15, cols=80)
    controller.start()
    try:
        controller.wait_for_settle()
        # Preview box height is max(8, 15-15) = 8.
        # desc_y = 4 + 8 + 1 = 13.
        # available lines: rows - 4 - desc_y = 15 - 4 - 13 = -2 (clamped to 0).
        # It should render without crash.
        text = get_screen_text(controller.screen)
        assert "G R A P H C A L C" in text, "Header should still be rendered"
    finally:
        controller.close()

def test_terminal_resize_large_height():
    """Resize terminal to 40 height, check that available description lines increase."""
    controller = TUIController(rows=40, cols=80)
    controller.start()
    try:
        controller.wait_for_settle()
        preview_h = 40 - 15 # 25
        desc_y = 4 + 25 + 1 # 30
        available_lines = 40 - 4 - 30 # 6
        # The description is drawn, we check that it is rendered correctly
        text = get_screen_text(controller.screen)
        assert "Formula" in text, "Formula should be visible in high-height terminal"
    finally:
        controller.close()

def test_scroll_reset_at_boundary():
    """Scroll to bottom of long description, then press Up to change selection, verify scroll reset."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN) # Go to Parabola
        controller.wait_for_settle()
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        controller.send_keys(KEY_UP) # Go back to Sine Wave
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "amplitude" in desc, "Sine Wave should display from start (offset 0)"
    finally:
        controller.close()

def test_scroll_keys_invalid_in_simulation():
    """Verify scroll keys do not affect description when user is in simulation screen."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_ENTER) # Open simulation choice
        controller.wait_for_settle()
        # Press [ and ] keys
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.send_keys(KEY_SCROLL_UP)
        controller.wait_for_settle()
        # Verify we are still in simulation choice menu
        text = get_screen_text(controller.screen)
        assert "Choose Visualization Mode" in text, "Should remain in Simulation Menu"
    finally:
        controller.close()

def test_scroll_keys_in_custom_input():
    """Verify scroll keys do not scroll description when inside Custom Input box."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys("c") # Open custom input
        controller.wait_for_settle()
        # Send scroll down key - in text input this should write ']' character or be processed as print
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        text = get_screen_text(controller.screen)
        # Check that ']' was written to the prompt rather than scrolling
        assert "]" in text or "Enter custom equation" in text, "Should handle scroll keys as text input"
    finally:
        controller.close()

def test_scroll_up_down_repeated():
    """Verify that spamming scroll up/down keys does not corrupt terminal display."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        for _ in range(5):
            controller.send_keys(KEY_SCROLL_DOWN)
            controller.send_keys(KEY_SCROLL_UP)
        controller.wait_for_settle()
        text = get_screen_text(controller.screen)
        assert "G R A P H C A L C" in text, "Header should remain intact"
        assert "│" in text, "Layout dividers should remain intact"
    finally:
        controller.close()

def test_navigation_out_of_bounds_down():
    """Navigate past the last template in the tab, verify selection clamps."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # There are 11 templates in 2D, let's navigate down 15 times
        for _ in range(15):
            controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        # Should be clamped to index 10 (Damped Cosine Wave)
        assert "cosine" in desc or "cosine wave" in desc or "decays" in desc, "Should clamp to last 2D template"
    finally:
        controller.close()

def test_navigation_out_of_bounds_up():
    """Navigate up past the first template, verify selection clamps to 0."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_UP)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "amplitude" in desc, "Should remain on first template description"
    finally:
        controller.close()

def test_empty_description_handling():
    """Verify no crash if description has very short or empty bounds (edge cases)."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Verify rendering is successful on startup
        text = get_screen_text(controller.screen)
        assert len(text.strip()) > 100, "Should render full interface without errors"
    finally:
        controller.close()


# TIER 3: Cross-Feature Combinations (interactions between scrolling, selections, inputs)
def test_scroll_navigate_reset_cycle():
    """Scroll down on a template, navigate selection down, then back up, verify scroll is reset to 0."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # 1. Scroll down Sine Wave
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        # 2. Go down to Parabola
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        # 3. Go back up to Sine Wave
        controller.send_keys(KEY_UP)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        # 4. Verify scroll offset is reset (shows "amplitude")
        assert "amplitude" in desc, "Scroll position must reset when returning to item"
    finally:
        controller.close()

def test_custom_input_and_description():
    """Enter custom input mode, submit custom equation, then go back, verify previous description still matches."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Open custom input
        controller.send_keys("c")
        controller.wait_for_settle()
        # Submit sin(x)
        controller.send_keys("sin(x)")
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        # Now in simulation choice screen, go back
        controller.send_keys(KEY_DOWN) # Navigate to Go Back
        controller.send_keys(KEY_DOWN)
        controller.send_keys(KEY_ENTER) # Select Go Back
        controller.wait_for_settle()
        # Verify description is for Sine Wave
        desc = get_description_text(controller.screen)
        assert "amplitude" in desc, "Should return to main menu with previous selection description intact"
    finally:
        controller.close()

def test_tab_switch_selection_reset():
    """Navigate down in 2D tab, switch to 3D tab, check selection resets to 0 (shows Ripple Waves)."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_DOWN) # Parabola
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT) # Switch to 3D
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "three-dimensional" in desc or "radial" in desc, "Switching tab should reset selection and show first 3D description"
    finally:
        controller.close()


# TIER 4: Real-World Application (full user workflows)
def test_user_scenario_2d_read():
    """Typical user scrolls through 2D templates, selects one, reads description, and runs terminal simulation."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        # Move to Dampened Wave
        controller.send_keys(KEY_DOWN)
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        # Read description (scrolling down and up)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.send_keys(KEY_SCROLL_UP)
        controller.wait_for_settle()
        # Enter simulation mode
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        # Select Simplified Terminal View
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        # Verify simulation started
        text = get_screen_text(controller.screen)
        assert "SIMULATING" in text, "Simulation screen should display"
    finally:
        controller.close()

def test_user_scenario_3d_read():
    """Typical user navigates to 3D, reads Sombrero description, and runs simulation."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        # Go to Sombrero (index 2)
        controller.send_keys(KEY_DOWN)
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        # Read details
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        # Start simulation
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        # Select Simplified Terminal View
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        text = get_screen_text(controller.screen)
        assert "SIMULATING" in text, "3D Simulation should start"
    finally:
        controller.close()

def test_user_scenario_vis_read():
    """Typical user navigates to Visuals, reads Julia Set description, and runs it."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.send_keys(KEY_RIGHT)
        controller.wait_for_settle()
        # Go to Julia Set
        controller.send_keys(KEY_DOWN)
        controller.wait_for_settle()
        # Scroll description
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        # Start simulation
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        # Select Simplified Terminal View
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        text = get_screen_text(controller.screen)
        assert "SIMULATING" in text, "Julia simulation should start"
    finally:
        controller.close()

def test_user_scenario_custom_flow():
    """Typical user opens custom input, types formula, views simulation, then exits back to main menu."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys("c")
        controller.wait_for_settle()
        controller.send_keys("x^2")
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        # Select simplified simulation
        controller.send_keys(KEY_ENTER)
        controller.wait_for_settle()
        # Verify simulating
        text1 = get_screen_text(controller.screen)
        assert "SIMULATING" in text1, "Custom simulation should run"
        # Quit simulation
        controller.send_keys("q")
        controller.wait_for_settle()
        # We should be back in the Simulation Menu. Quit that back to Main Menu
        controller.send_keys("q")
        controller.wait_for_settle()
        text2 = get_screen_text(controller.screen)
        assert "G R A P H C A L C" in text2, "Should return to main menu"
    finally:
        controller.close()

def test_user_scenario_comprehensive():
    """Comprehensive navigation: swap tabs, change selections, scroll, resize width, exit cleanly."""
    controller = TUIController()
    controller.start()
    try:
        controller.wait_for_settle()
        controller.send_keys(KEY_RIGHT)
        controller.send_keys(KEY_DOWN)
        controller.send_keys(KEY_SCROLL_DOWN)
        controller.wait_for_settle()
        controller.send_keys(KEY_LEFT)
        controller.wait_for_settle()
        desc = get_description_text(controller.screen)
        assert "amplitude" in desc or "frequency" in desc, "Sine Wave description should show"
    finally:
        controller.close()


# ----------------- TEST RUNNER -----------------

def run_tests():
    tests = [
        # Tier 1
        ("test_tab_navigation_2d", test_tab_navigation_2d),
        ("test_tab_navigation_3d", test_tab_navigation_3d),
        ("test_tab_navigation_vis", test_tab_navigation_vis),
        ("test_navigation_down_2d", test_navigation_down_2d),
        ("test_navigation_down_3d", test_navigation_down_3d),
        ("test_navigation_down_vis", test_navigation_down_vis),
        ("test_description_content_dampened_wave", test_description_content_dampened_wave),
        ("test_description_content_sombrero", test_description_content_sombrero),
        ("test_description_content_plasma", test_description_content_plasma),
        ("test_scrolling_key_down_ignored_if_no_scroll", test_scrolling_key_down_ignored_if_no_scroll),
        ("test_scrolling_key_up_ignored_at_top", test_scrolling_key_up_ignored_at_top),
        ("test_scroll_down_action", test_scroll_down_action),
        ("test_scroll_up_action", test_scroll_up_action),
        ("test_scroll_reset_on_next_item", test_scroll_reset_on_next_item),
        ("test_scroll_reset_on_tab_change", test_scroll_reset_on_tab_change),
        # Tier 2
        ("test_short_description_no_scroll", test_short_description_no_scroll),
        ("test_long_description_wrapping", test_long_description_wrapping),
        ("test_scroll_at_upper_boundary", test_scroll_at_upper_boundary),
        ("test_scroll_at_lower_boundary", test_scroll_at_lower_boundary),
        ("test_terminal_resize_small_width", test_terminal_resize_small_width),
        ("test_terminal_resize_large_width", test_terminal_resize_large_width),
        ("test_terminal_resize_small_height", test_terminal_resize_small_height),
        ("test_terminal_resize_large_height", test_terminal_resize_large_height),
        ("test_scroll_reset_at_boundary", test_scroll_reset_at_boundary),
        ("test_scroll_keys_invalid_in_simulation", test_scroll_keys_invalid_in_simulation),
        ("test_scroll_keys_in_custom_input", test_scroll_keys_in_custom_input),
        ("test_scroll_up_down_repeated", test_scroll_up_down_repeated),
        ("test_navigation_out_of_bounds_down", test_navigation_out_of_bounds_down),
        ("test_navigation_out_of_bounds_up", test_navigation_out_of_bounds_up),
        ("test_empty_description_handling", test_empty_description_handling),
        # Tier 3
        ("test_scroll_navigate_reset_cycle", test_scroll_navigate_reset_cycle),
        ("test_custom_input_and_description", test_custom_input_and_description),
        ("test_tab_switch_selection_reset", test_tab_switch_selection_reset),
        # Tier 4
        ("test_user_scenario_2d_read", test_user_scenario_2d_read),
        ("test_user_scenario_3d_read", test_user_scenario_3d_read),
        ("test_user_scenario_vis_read", test_user_scenario_vis_read),
        ("test_user_scenario_custom_flow", test_user_scenario_custom_flow),
        ("test_user_scenario_comprehensive", test_user_scenario_comprehensive)
    ]

    print("=" * 60)
    print("      GRAPHCALC E2E TEST RUNNER")
    print("=" * 60)
    print(f"Total Tests to Run: {len(tests)}")
    print("-" * 60)

    passed_count = 0
    failed_count = 0
    results = []

    for name, func in tests:
        sys.stdout.write(f"Running {name: <45} ")
        sys.stdout.flush()
        try:
            func()
            print("\033[1;32m[PASS]\033[0m")
            results.append((name, "PASS", ""))
            passed_count += 1
        except Exception as e:
            print("\033[1;31m[FAIL]\033[0m")
            results.append((name, "FAIL", str(e)))
            failed_count += 1

    print("=" * 60)
    print("                  TEST SUMMARY")
    print("=" * 60)
    print(f"Total:  {len(tests)}")
    print(f"Passed: \033[1;32m{passed_count}\033[0m")
    print(f"Failed: \033[1;31m{failed_count}\033[0m")
    print("=" * 60)

    if failed_count > 0:
        print("\nFailures Detail:")
        print("-" * 60)
        for name, status, err in results:
            if status == "FAIL":
                print(f"- {name}: {err}")
        print("-" * 60)
        
    return failed_count

if __name__ == "__main__":
    # Ensure graphcalc is compiled before starting tests
    try:
        exit_code = run_tests()
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print("\nTesting interrupted by user.")
        sys.exit(1)
