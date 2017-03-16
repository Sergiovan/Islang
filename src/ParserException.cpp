//
// Created by Sergiovan on 08-Mar-17.
//

#include "ParserException.h"

namespace ns_parser {
    ParserException::ParserException(std::string message) : IslangException(message) {}
}