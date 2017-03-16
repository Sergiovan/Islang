//
// Created by Sergiovan on 05-Mar-17.
//

#include "Interpreter.h"
#include "Variable.h"


namespace ns_variable {

    Variable::Variable() : type(ns_ast::NONE), value(nullptr), c_value(nullptr), computed(false){

    }

    Variable::Variable(ns_ast::AST* value, ns_ast::node_type type) : type(type), value(value), c_value(nullptr),
                                                                     computed(false){

    }

    ns_ast::node_type Variable::get_type(){
        return type;
    }

    void Variable::set_value(ns_ast::AST* value){
        Variable::value = value;
    }

    ns_ast::AST* const Variable::get_raw_value(){
        return value;
    }

    ns_ast::AST* Variable::get_value(ns_interpreter::Interpreter& itp){
        if(!computed) {
            c_value = itp.interpret(value, true);
            computed = true;
        }
        return c_value;
    }

    void Variable::reset(){
        if(c_value != value) {
            delete c_value;
        }
        c_value = nullptr;
        computed = false;
    }

}