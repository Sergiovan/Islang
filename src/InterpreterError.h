//
// Created by Sergiovan on 11-Mar-17.
//

#ifndef ISLANG_INTERPRETERERROR_H
#define ISLANG_INTERPRETERERROR_H

#include "IslangException.h"

namespace ns_interpreter {
    class InterpreterError : public ns_exception::IslangException {
    public:
        InterpreterError(const std::string &message);
        using ns_exception::IslangException::what;
    };
}

#endif //ISLANG_INTERPRETERERROR_H
