//
// Created by Sergiovan on 06-Mar-17.
//

#include "ParserPromise.h"
#include "ParserException.h"

namespace ns_parser {

    ParserPromise::ParserPromise(std::function<ns_ast::AST_p()> func, ns_lexer::Token& token, ns_ast::AST_p node)
    : func(func), token(token), node(node), evaluated(false){

    }

    ParserPromise::operator ns_ast::AST_p() {
        if(good()){
            return node;
        }else{
            throw ns_parser::ParserException(token.error("Unexpected token")); //TODO
        }
    }

    ParserPromise& ParserPromise::operator||(ParserPromise &&rhs) {
        return good() ? *this : rhs;
    }

    bool ParserPromise::good() {
        if(!evaluated){
            node = func();
            evaluated = true;
        }
        return node != nullptr;
    }
}