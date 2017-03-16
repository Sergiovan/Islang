//
// Created by Sergiovan on 04-Mar-17.
//

#ifndef ISLANG_TOKENIZER_H
#define ISLANG_TOKENIZER_H

#include <vector>
#include "Token.h"
#include "Scanner.h"

namespace ns_lexer {
    class Lexer {
    public:
        Lexer();
        std::vector<Token> read_file(std::string filename);
        std::vector<Token> read_string(std::string string);
        bool okay();
    private:
        Token next(ns_scanner::Scanner& sc);
        std::vector<Token> read(ns_scanner::Scanner& sc);
        bool new_line, error;
        int nesting;
    };
}


#endif //ISLANG_TOKENIZER_H
