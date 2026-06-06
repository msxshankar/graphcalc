#ifndef GRAPHCALC_TUI_HPP
#define GRAPHCALC_TUI_HPP

#include <string>
#include <vector>
#include "parser.hpp"

struct GraphTemplate {
    std::string name;
    std::string equation;
    bool is3D;
    std::string description;
};

struct TUICell {
    std::string ch = " ";
    std::string color = "";
};

class TUI {
public:
    TUI();
    ~TUI();

    // Runs the main TUI loop. Returns 0 on clean exit.
    int run();

private:
    // Grid rendering helpers
    void clearGrid();
    void drawText(int x, int y, const std::string& text, const std::string& color = "");
    void drawBox(int x, int y, int w, int h, const std::string& color = "");
    void flushScreen();
    void renderMiniPreview(int startX, int startY, int w, int h, const std::string& eq, bool is3D, double time);

    // Screen management
    void showMenu();
    void showSimulationMenu(const std::string& equation, bool is3D);
    void runTerminalSimulation(const std::string& equation, bool is3D);

    // Render helpers
    void clearScreen();
    void drawHeader();
    void drawFooter(const std::string& info);

    // Setup / Cleanup
    void enterAltBuffer();
    void exitAltBuffer();
    void setupTerminal();
    void restoreTerminal();

    // Input parsing helpers
    int readKey(bool nonBlocking = false);
    std::string handleCustomInput(bool is3D);

    // List of templates
    std::vector<GraphTemplate> templates2D;
    std::vector<GraphTemplate> templates3D;
    std::vector<GraphTemplate> templatesVis;
    int selectedIndex;
    int tabIndex; // 0 = 2D, 1 = 3D, 2 = Visualisations

    // Double-buffered grid and menu animation parameters
    std::vector<std::vector<TUICell>> screenGrid;
    int screenWidth;
    int screenHeight;
    double menuTimeVal;
    int scrollOffset;
    int maxScrollOffset;

    void updateTerminalSize();

    // Terminal raw mode storage
    void* origTermiosPtr; // struct termios*
};


#endif // GRAPHCALC_TUI_HPP
