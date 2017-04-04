//
// Created by Sergiovan on 05-Mar-17.
//

#ifndef ISLANG_AST_H
#define ISLANG_AST_H

#include <map>
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
        float                       n = -1;   //Number
        bool                        b = false;   //Boolean
        std::string                 s = "";   //String
        int                         v = -1;   //Enum value
        //ns_variable::Variable*      var = nullptr; //Variables TODO Fix
        std::vector<AST*>           cs = {};  //Complex string
        std::map<std::string, int>  e = {};   //Enum
        list_node                   l = {node_type::NONE, {}};   //List
        unary_node                  un = {"", nullptr};  //Unary
        binary_node                 bin = {"", nullptr, nullptr}; //Binary
        block_node                  blk = {NONE_BLOCK, "", {}};
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

        template <node_type type>
            bool is() const noexcept {
            return type == AST::type;
        }

        template <node_type fst, node_type scd, node_type... rest>
            bool is() const noexcept {
            return fst == type || is<scd, rest...>();
        }

        bool is_num() const noexcept;
        bool is_bool() const noexcept;
        bool is_str() const noexcept;
        bool is_cstr() const noexcept;
        bool is_any_str() const noexcept;
        bool is_val() const noexcept;
        bool is_enum() const noexcept;
        bool is_list() const noexcept;
        bool is_un() const noexcept;
        bool is_bin() const noexcept;
        bool is_blk() const noexcept;

        //TODO delete function

        /*virtual ~AST() = default;
        AST(const AST& other) = default;
        AST(AST&& other) = default;
        AST& operator= (const AST& other) = default;
        AST& operator= (AST&& other) = default;*/
    };

    void indent(int depth);
}


#endif //ISLANG_AST_H
