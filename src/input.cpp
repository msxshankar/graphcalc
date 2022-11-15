//
// Created by mayur on 12/11/22.
//

#include <iostream>
#include <string>
#include "input.hpp"

int Input::values() {
    int index = linEquation.find("x");
    std::cout << index;

    return index;
}