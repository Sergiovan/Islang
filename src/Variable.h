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
        Variable(ns_ast::AST_p value, ns_ast::node_type type);

        ns_ast::node_type get_type();
        void set_value(ns_ast::AST_p value);

        ns_ast::AST_p const get_raw_value();
        ns_ast::AST_p get_value(ns_interpreter::Interpreter& itp);

        void reset();

    private:
        ns_ast::node_type type;
        ns_ast::AST_p value;
        ns_ast::AST_p c_value;

        bool computed;
    };
}


#endif //ISLANG_VARIABLE_H
