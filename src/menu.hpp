#ifndef GRAPHCALC_MENU_HPP

#define GRAPHCALC_MENU_HPP

#endif GRAPHCALC_MENU_HPP


class Menu {

public:
    std::string linEquation;
    double y, x, c;

    Menu() {
        linEquation = "";
        y = 1;
        x = 5;
        c = 6;
    }

    int values();
    int welcome();
    int validation();
    int userInput();
};
