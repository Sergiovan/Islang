//
// Created by Sergiovan on 05-Mar-17.
//

#include <string>
#include "Color.h"

namespace color {

    std::string ansi(int code){
        return "\033[" + std::to_string(code) + "m";
    }

}