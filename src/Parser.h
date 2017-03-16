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
        ns_ast::AST* read_file(std::string filename);
        ns_ast::AST* read_string(std::string string);
    private:
        void sanitize();
        ns_ast::AST* run();

        void read();
        int whitespace();
        ns_lexer::Token& token();

        ParserPromise find(std::function<ns_ast::AST*(Parser* parser)> what);

        ns_ast::AST* boolean();
        ns_ast::AST* number();
        ns_ast::AST* string();
        ns_ast::AST* identifier();
        ns_ast::AST* list_element();
        ns_ast::AST* list();
        ns_ast::AST* enumeration_name();
        ns_ast::AST* enumeration();
        ns_ast::AST* literal();

        ns_ast::AST* list_expr_parens();
        ns_ast::AST* list_expr_pick();
        ns_ast::AST* list_expr_enum();
        ns_ast::AST* list_expr_index();
        ns_ast::AST* list_expr();

        ns_ast::AST* boolean_expr_parens();
        ns_ast::AST* boolean_expr_unary();
        ns_ast::AST* boolean_expr_binary();
        ns_ast::AST* boolean_expr_comparison();
        ns_ast::AST* boolean_expr();

        ns_ast::AST* value();
        ns_ast::AST* if_expr();
        ns_ast::AST* expression();

        ns_ast::AST* import_statement();
        ns_ast::AST* declaration_statement();
        ns_ast::AST* assignment_statement();
        ns_ast::AST* keyword_statement();
        ns_ast::AST* use_statement();
        ns_ast::AST* repr_statement();
        ns_ast::AST* block_statement();
        ns_ast::AST* location_statement();
        ns_ast::AST* generate_statement();

        ns_ast::AST* statement();

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
