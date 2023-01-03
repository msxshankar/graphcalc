#ifndef GRAPHCALC_MENU_HPP

#define GRAPHCALC_MENU_HPP

#endif //GRAPHCALC_MENU_HPP


class Menu {

public:
    std::string linEquation;
    double y, m, c;
    int choiceNumber;

    // Default constructor to initialise basic values
    Menu() {
        linEquation = "";
        y = 0;
        m = 0;
        c = 0;
        choiceNumber = 0;
    }

    // Method stubs - to be implemented at a later date
    int welcome();
    int choice();
    //int validation();
    int showGraph();
};
