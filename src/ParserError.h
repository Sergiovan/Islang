//
// Created by Sergiovan on 08-Mar-17.
//

#ifndef ISLANG_PARSERERROR_H
#define ISLANG_PARSERERROR_H


#include "IslangException.h"

namespace ns_parser{

    class ParserError :
    public ns_exception::IslangException {
        public:
        ParserError(std::string message);
        using ns_exception::IslangException::what;
    };

}

#endif //ISLANG_PARSERERROR_H
