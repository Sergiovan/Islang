//
// Created by Sergiovan on 04-Mar-17.
//

#ifndef ISLANG_ISLANGXCEPTION_H
#define ISLANG_ISLANGXCEPTION_H

#include <exception>
#include <string>

namespace ns_exception {
    class IslangException : public std::exception {
    public:
        IslangException(std::string message);
        virtual char const * what() const noexcept;
    protected:
        std::string message;
    };
}


#endif //ISLANG_ISLANGEXCEPTION_H
