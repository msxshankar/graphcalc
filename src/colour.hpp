// msxshankar - 2023

#ifndef GRAPHCALC_COLOUR_HPP
#define GRAPHCALC_COLOUR_HPP
#endif //GRAPHCALC_COLOUR_HPP

// Terminal colours and styles

#define RESET "\x1B[0m"

#define FGRED(x) "\x1B[31m" x RESET
#define FGGREEN(x) "\x1B[32m" x RESET
#define FGYELLOW(x) "\x1B[33m" x RESET
#define FGBLUE(x) "\x1B[314" x RESET
#define FGWHITE(x) "\x1B[37m" x RESET

#define BOLD(x) "\x1B[1m" x RESET