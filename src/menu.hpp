// Class for graphcalc menu system

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
