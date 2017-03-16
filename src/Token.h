//
// Created by Sergiovan on 04-Mar-17.
//

#ifndef ISLANG_TOKEN_H
#define ISLANG_TOKEN_H

#include "Character.h"
#include "Grammar.h"

namespace ns_lexer {
    class Token {
    public:
        Token();
        Token(ns_scanner::Character c);
        Token operator+(char other) const;
        Token operator+(ns_scanner::Character& other) const;
        operator std::string() const;
        std::string to_string() const;
        std::string print() const;
        std::string get_full_line() const;
        std::string get_full_line_pointer() const;
        std::string error(std::string message = "") const;
        void die(std::string message = "") const;

        void set_type(grammar::token_type new_type);
        grammar::token_type get_type() const;

        int get_index() const;
        int get_line() const;
        int get_col() const;
        static const std::string table_header;
    private:
        std::string content;
        ns_scanner::sp_ifstream file;
        ns_scanner::sp_sstream  string;
        bool is_file;
        int index, line, col, line_start;
        grammar::token_type type;
    };
}


#endif //ISLANG_TOKEN_H
