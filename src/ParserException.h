//
// Created by Sergiovan on 08-Mar-17.
//

#ifndef ISLANG_PARSEREXCEPTION_H
#define ISLANG_PARSEREXCEPTION_H

#include "IslangException.h"
namespace ns_parser {
    class ParserException : public ns_exception::IslangException {
    public:
        ParserException(std::string message);
        using ns_exception::IslangException::what;
    };
}

#endif //ISLANG_PARSEREXCEPTION_H
