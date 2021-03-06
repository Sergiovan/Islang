//
// Created by Sergiovan on 11-Mar-17.
//

#ifndef ISLANG_INTERPRETER_H
#define ISLANG_INTERPRETER_H

#include <utility>
#include <random>
#include <string>

#include "AST.h"
#include "Scope.h"
#include "Variable.h"

namespace ns_interpreter {

    class Interpreter {
    public:
        Interpreter(std::string cwd);
        //TODO Destructor, rule of 5

        ns_ast::AST_p interpret(ns_ast::AST_p node, bool all_the_way = false);
        void generate(std::string what = "");

        std::string stringified(ns_ast::AST_p node);
        bool truthyness(ns_ast::AST_p node);
        bool equal(ns_ast::AST_p left, ns_ast::AST_p right);

        float random_number(float limit = 100);
        int random_int(int limit = 100);

        static std::string error(ns_ast::AST_p node, std::string message);
        static void die(ns_ast::AST_p node, std::string message);
    private:
        void _generate(Scope* s, int startlevel = 0);

        ns_ast::AST_p i_cstring(ns_ast::AST_p node);
        ns_ast::AST_p i_list(ns_ast::AST_p node);
        bool l_elem_conforms(ns_ast::AST_p node);
        bool l_elem_conforms(ns_ast::AST_p node, int level, ns_ast::node_type type, std::vector<bool>& prob);
        ns_ast::AST_p i_enumeration(ns_ast::AST_p node);
        ns_ast::AST_p i_variable(ns_ast::AST_p node);
        ns_ast::AST_p i_unary(ns_ast::AST_p node);
        ns_ast::AST_p i_binary(ns_ast::AST_p node);
        ns_ast::AST_p i_block(ns_ast::AST_p node);

        varscope get_var_scope(std::string name, ns_ast::AST_p node, bool var_and_scope = false);
        varscope get_var(std::string name, ns_ast::AST_p node);
        Scope* get_scope(std::string name, ns_ast::AST_p node);

        void go_in(std::string name);
        void go_out();
        std::string scope_name();

        std::string cwd;
        Scope* global, *current, *special, *start;
        std::vector<std::string> current_name;

        int import = 0;

        std::mt19937 mt;
    };
}


#endif //ISLANG_INTERPRETER_H
