//
// Created by Sergiovan on 11-Mar-17.
//

#ifndef ISLANG_INTERPRETEREXCEPTION_H
#define ISLANG_INTERPRETEREXCEPTION_H

#include "IslangException.h"

namespace ns_interpreter {

    class InterpreterException : public ns_exception::IslangException {
    public:
        InterpreterException(const std::string &message);

        using ns_exception::IslangException::what;
    };

}

#endif //ISLANG_INTERPRETEREXCEPTION_H
