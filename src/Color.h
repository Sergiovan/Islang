//
// Created by Sergiovan on 05-Mar-17.
//

#ifndef ISLANG_COLOR_H
#define ISLANG_COLOR_H

#include <iostream>
#include <string>

namespace color {
    enum Code {
        ALL_RESET   = 0,
        FG_BLACK    = 30,
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_MAGENTA  = 35,
        FG_CYAN     = 36,
        FG_WHITE    = 37,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        FG_BBLACK    = 90,
        FG_BRED      = 91,
        FG_BGREEN    = 92,
        FG_BYELLOW   = 93,
        FG_BBLUE     = 94,
        FG_BMAGENTA  = 95,
        FG_BCYAN     = 96,
        FG_BWHITE    = 97
    };

    std::string ansi(int code);
}

#endif //ISLANG_COLOR_H
