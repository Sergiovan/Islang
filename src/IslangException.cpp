//
// Created by Sergiovan on 04-Mar-17.
//

#include "IslangException.h"

using namespace ns_exception;

IslangException::IslangException(std::string message) {
    IslangException::message = message;
}

char const* IslangException::what() const noexcept {
    return message.c_str();
}