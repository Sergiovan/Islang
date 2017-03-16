//
// Created by Sergiovan on 08-Mar-17.
//

#include <string>
#include "TokenException.h"

namespace ns_lexer {
    TokenException::TokenException(std::string message) : IslangException(message) {}
}