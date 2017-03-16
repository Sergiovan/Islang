//
// Created by Sergiovan on 05-Mar-17.
//

#ifndef ISLANG_VARIABLE_H
#define ISLANG_VARIABLE_H

#include "AST.h"

namespace ns_interpreter {
    class Interpreter;
}

namespace ns_variable {
    class Variable {
    public:
        Variable();
        Variable(ns_ast::AST* value, ns_ast::node_type type);

        ns_ast::node_type get_type();
        void set_value(ns_ast::AST* value);

        ns_ast::AST* const get_raw_value();
        ns_ast::AST* get_value(ns_interpreter::Interpreter& itp);

        void reset();

    private:
        ns_ast::node_type type;
        ns_ast::AST* value;
        ns_ast::AST* c_value;

        bool computed;
    };
}


#endif //ISLANG_VARIABLE_H
