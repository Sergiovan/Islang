//
// Created by Sergiovan on 05-Mar-17.
//

#ifndef ISLANG_AST_H
#define ISLANG_AST_H

#include <string>
#include <vector>
#include "Token.h"

namespace ns_variable {
    class Variable;
}

namespace ns_ast {

    struct AST;

    enum node_type {
        NUMBER,
        BOOLEAN,
        STRING,
        VARIABLE,
        VALUE,
        CSTRING,
        LIST,
        ENUM,
        BINARY,
        UNARY,
        BLOCK,
        NONE
    };

    enum block_type {
        LOCATION,
        EVENT,
        CONTAINS,
        REPR,
        PROGRAM,
        NONE_BLOCK
    };

    struct list_node{
        node_type        type;
        std::vector<AST*> nodes;
    };

    struct unary_node {
        std::string op;
        AST* operand;
    };

    struct binary_node {
        std::string op;
        AST* left;
        AST* right;
    };

    struct block_node {
        block_type type;
        std::string name;
        std::vector<AST*> statements;
    };

    struct value { //Should be union, alas, cannot be union :(
        float                    n = -1;   //Number
        bool                     b = false;   //Boolean
        std::string              s = "";   //String
        int                      v = -1;   //Enum value
        ns_variable::Variable*   var = nullptr; //Variables
        std::vector<AST*>        cs = {};  //Complex string
        std::vector<std::string> e = {};   //Enum
        list_node                l = {node_type::NONE, {}};   //List
        unary_node               un = {"", nullptr};  //Unary
        binary_node              bin = {"", nullptr, nullptr}; //Binary
        block_node               blk = {NONE_BLOCK, "", {}}; //Block
    };

    struct AST {
        node_type type;
        value val;
        ns_lexer::Token token;
        int offset_left, offset_right;

        AST(ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(float n, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(bool b, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(std::string s, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(ns_variable::Variable *var, std::string varname, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(int v, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(std::vector<AST*>&& vec, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(std::vector<AST*>&& vec, node_type type, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(std::vector<std::string>&& e, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(std::string op, AST* operand, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(std::string op, AST *left, AST *right, ns_lexer::Token token, int oleft = 0, int oright = 0);
        AST(block_type type, std::string name, std::vector<AST *> &&statements, ns_lexer::Token token,
            int oleft = 0, int oright = 0);
        void print(int depth = 0);
    };

    void indent(int depth);
}


#endif //ISLANG_AST_H