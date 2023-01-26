// Class for graphcalc menu system

// Terminal colours
enum colours {};

class Menu {

private:
    int choiceNumber;

public:

    // Constructor to initialise basic values
    Menu() : choiceNumber(0) {};

    // Methods for menu system
    void welcome();
    int choice();
    int showGraph();
};
