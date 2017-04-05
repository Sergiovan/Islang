//
// Created by Sergiovan on 05-Mar-17.
//

#ifndef ISLANG_PARSER_H
#define ISLANG_PARSER_H

#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "AST.h"
#include "Lexer.h"
#include "Token.h"
#include "ParserPromise.h"
#include "ParserException.h"

namespace ns_parser {

    class Parser {
    public:
        Parser();
        ns_ast::AST_p read_file(std::string filename);
        ns_ast::AST_p read_string(std::string string); //TODO Errors
    private:
        void sanitize();
        ns_ast::AST_p run(bool string = false);

        void read();
        int whitespace();
        ns_lexer::Token& token();

        ParserPromise find(std::function<ns_ast::AST_p(Parser* parser)> what);

        ns_ast::AST_p boolean();
        ns_ast::AST_p number();
        ns_ast::AST_p string();
        ns_ast::AST_p identifier();
        ns_ast::AST_p list_element();
        ns_ast::AST_p list();
        ns_ast::AST_p enumeration_name();
        ns_ast::AST_p enumeration();
        ns_ast::AST_p literal();

        ns_ast::AST_p list_expr_parens();
        ns_ast::AST_p list_expr_pick();
        ns_ast::AST_p list_expr_chancify();
        ns_ast::AST_p list_expr_index();
        ns_ast::AST_p list_expr();

        ns_ast::AST_p boolean_expr_parens();
        ns_ast::AST_p boolean_expr_unary();
        ns_ast::AST_p boolean_expr_binary();
        ns_ast::AST_p boolean_expr_comparison();
        ns_ast::AST_p boolean_expr();

        ns_ast::AST_p value();
        ns_ast::AST_p if_expr();
        ns_ast::AST_p expression();

        ns_ast::AST_p import_statement();
        ns_ast::AST_p declaration_statement();
        ns_ast::AST_p assignment_statement();
        ns_ast::AST_p keyword_statement();
        ns_ast::AST_p use_statement();
        ns_ast::AST_p repr_statement();
        ns_ast::AST_p block_statement();
        ns_ast::AST_p generate_statement();

        ns_ast::AST_p statement();

        bool found(std::string content);
        bool found(grammar::token_type type);
        bool found(grammar::token_subtype subtype);

        void expect(std::string content);
        void expect(grammar::token_type type);
        void expect(grammar::token_subtype subtype);

        int firstof(std::vector<std::string> elements);
        int lastof(std::vector<std::string> elements);
        int next();
        int csnext();
        int line_end();

        void push(int ll, int rl);
        void push(std::pair<int, int> limit);
        std::pair<int, int> pop();

        std::vector<ns_lexer::Token> tokens;
        int index, ll, rl;
        std::stack<std::pair<int, int>> limits;
        std::stack<ParserException> ex_stack;

        std::string error(std::string message);

        //State
        bool was_cond_repr = false;
        bool next_was_cond_repr = false;
    };

}


#endif //ISLANG_PARSER_H
