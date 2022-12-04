#ifndef GRAPHCALC_MENU_HPP

#define GRAPHCALC_MENU_HPP

#endif GRAPHCALC_MENU_HPP


class Menu {

public:
    std::string linEquation;
    double y, m, c;

    Menu() {
        linEquation = "";
        y = 0;
        m = 0;
        c = 0;
    }

    int values();
    int welcome();
    int validation();
    int linearUserInput();
};
