// msxshankar 2023
// testing all key functions

#include <iostream>
#include <memory>

#include "gtest/gtest.h"
#include "graphcalc_test.hpp"
#include "../src/main.hpp"
#include "../src/menu.hpp"
#include "../src/linear.hpp"


#define SUCCESS 0
#define FAIL 1
#define UNDEFINED 2

/*
int argumentTest () {
    struct arguments {
        const std::string argument1 = "-h";
        const std::string argument2 = "--help";
        const std::string argument3 = "-v";
        const std::string argument4 = "--version";
        const std::string argument5 = "-";
        const std::string argument6 = "--";
        const std::string argument7 = "739djj";
        const std::string argument8 = "";
        const std::string argument9 = "-v93";
        const std::string argument10 = "--help2";
    };

    struct arguments *ptr;
    if (argumentParser(ptr->argument5)) {
        return FAIL;
    }
    else {
        return SUCCESS;
    }
}
*/

// Menu testing
/*
int menu () {
   std::unique_ptr<Menu>testMenu(new Menu());

   // Test menu object is created properly
   if (testMenu == nullptr) {
       return FAIL;
   }

   testMenu->choice();
}
*/
TEST (MenuTest, InvalidInput) {
    //std::unique_ptr<Menu>testMenu(new Menu());

    //EXPECT_EQ(testMenu->choice(), 0);

    Menu menu1;
    EXPECT_EQ(menu1.choice(), 0);
}

