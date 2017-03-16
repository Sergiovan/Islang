#ifndef ISLANG_TOKENEXCEPTION_H
#define ISLANG_TOKENEXCEPTION_H

#include <string>
#include "IslangException.h"


namespace ns_lexer {
    class TokenException : public ns_exception::IslangException {
    public:
        TokenException(std::string message);
        using ns_exception::IslangException::what;
    };
}

#endif