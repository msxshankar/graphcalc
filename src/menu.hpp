// Class for graphcalc menu system

class Menu {

private:
    int choiceNumber;
    double m = 0;
    double c = 0;
    double y_in = 1;

    void interactiveLinear();
    void interactive3D();

    template<typename T>
    T getInput(const std::string& prompt, T defaultValue);

public:

    // Default constructor to initialise basic values
    Menu() {
        choiceNumber = 0;
    }

    void welcome();
    int choice();
    int showGraph();
};
