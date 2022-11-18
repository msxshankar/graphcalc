// Class definitions

class Input {
public:
    std::string linEquation;

    double y;
    double x;
    double c;

    Input() {
        linEquation = "";
        y = 1;
        x = 5;
        c = 6;
    }

    int values();
};