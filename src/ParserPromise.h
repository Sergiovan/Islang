//
// Created by Sergiovan on 06-Mar-17.
//

#ifndef ISLANG_PARSERPROMISE_H
#define ISLANG_PARSERPROMISE_H

#include <functional>

#include "AST.h"
#include "Token.h"

namespace ns_parser {
    class ParserPromise {
    public:
        ParserPromise(std::function<ns_ast::AST_p()> func, ns_lexer::Token& token, ns_ast::AST_p node = nullptr);
        operator ns_ast::AST_p();
        ParserPromise& operator||(ParserPromise&& rhs);
        bool good();
    private:
        std::function<ns_ast::AST_p()> func;
        ns_lexer::Token& token;
        ns_ast::AST_p node;
        bool evaluated;
    };
}


#endif //ISLANG_PARSERPROMISE_H
