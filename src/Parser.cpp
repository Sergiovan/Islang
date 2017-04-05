//
// Created by Sergiovan on 05-Mar-17.
//

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "Color.h"
#include "Parser.h"
#include "ParserException.h"
#include "ParserError.h"

namespace ns_parser {

    using namespace ns_ast;

    Parser::Parser() : tokens(), index(0), ll(0), rl(0), limits(){

    }

    AST_p Parser::read_file(std::string filename) {
        ns_lexer::Lexer lex;
        tokens = lex.read_file(filename);
        if(!lex.okay()){
            std::cout << std::endl << color::ansi(color::FG_BRED) << "Error during lexing, aborting."
                      << color::ansi(color::ALL_RESET) << std::endl;
            return nullptr;
        }
        rl = (int) tokens.size();
        return run();
    }

    AST_p Parser::read_string(std::string string) {
        ns_lexer::Lexer lex;
        tokens = lex.read_string(string);
        if(!lex.okay()){
            std::cout << std::endl << color::ansi(color::FG_BRED) << "Error during lexing, aborting."
                      << color::ansi(color::ALL_RESET) << std::endl;
            return nullptr;
        }
        rl = (int) tokens.size();
        return run(true);
    }

    void Parser::sanitize(){
        int depth = 0;
        std::string opener, closer;
        for(auto& token : tokens){
            if(token.get_type() == grammar::token_type::COMMENT){
                token.set_type(grammar::token_type::NONE);
            }
            if(depth == 0 && grammar::is(token.to_string(), grammar::token_subtype::SYMBOL_OPENER)){
                depth++;
                opener = token.to_string();
                switch(opener[0]){
                    case '(':
                        closer = std::string(")");
                        break;
                    case '[':
                        closer = std::string("]");
                        break;
                    case '{':
                        closer = std::string("}");
                        break;
                    default:
                        throw ParserException("Illegal opening symbol while sanitizing: " + opener);
                }
            }else if(depth > 0 && token.to_string() == opener){
                depth++;
            }
            if(depth > 0 && token.to_string() == closer){
                depth--;
            }
            if(depth > 0 && (token.get_type() == grammar::token_type::WHITESPACE ||
                             token.get_type() == grammar::token_type::NEWLINE)){
                token.set_type(grammar::token_type::NONE);
            }
        }
    }

    AST_p Parser::run(bool string) {
        try {
            sanitize();
            if(!string) {
                std::vector<AST_p > stmts;
                int as = next();
                index = as;
                while(!found(grammar::token_type::END)){
                    stmts.push_back((AST_p ) find(&Parser::statement));
                    if (found(grammar::token_type::END)) {
                        break;
                    }
                    expect(grammar::token_type::NEWLINE);
                    index = next();
                }
                return make_ast(ns_ast::PROGRAM, "", std::move(stmts), tokens[0]);
            }else{
                AST_p ret = find(&Parser::statement);
                return ret;
            }
        }catch(ParserException& ex){
            while(ex_stack.size() > 0){
                ParserException& e = ex_stack.top();
                std::cout << e.what() << std::endl;
                ex_stack.pop();
            }
            std::cout << ex.what() << std::endl;
        }catch(ParserError& ex){
            std::cout << ex.what() << std::endl;
        }catch(...){
            std::cout << "Unknown exception caught, no idea what it means" << std::endl;
        }
        return nullptr;
    }

    void Parser::read(){
        index++;
        while(index < rl && tokens[index].get_type() == grammar::token_type::NONE){
            index++;
        }
        if(index > rl || index < ll){
            std::stringstream ss;
            ss << "Read (" << index << ") outside of bounds [" << ll << ", " << rl << "]";
            throw ParserError(error(ss.str()));
        }
    }

    int Parser::whitespace(){
        int whitespaces = 0;
        while(found(grammar::token_type::WHITESPACE)){
            whitespaces++;
            read();
        }
        return whitespaces;
    }

    ns_lexer::Token& Parser::token(){
        return tokens[index];
    }

    ParserPromise Parser::find(std::function<AST_p (Parser* parser)> what) {
        auto esize = ex_stack.size();
        return ParserPromise([this, &what, esize](){
            AST_p node = nullptr;
            int pin = this->index;
            auto psize = this->limits.size();
            try {
                node = what(this);
                while(esize < this->ex_stack.size()){
                    this->ex_stack.pop();
                }
            }catch(ParserException& ex){
                node = nullptr;
                this->index = pin;
                this->ex_stack.push(ex);
            }
            while(psize < this->limits.size()){ //TODO Better way of doing this?
                this->pop();
            }
            return node;
        }, tokens[index]);
    }

    AST_p Parser::boolean() {
        using namespace grammar;
        if(!found(token_subtype::KEYWORD_BOOLEAN) && !found(token_type::BOOLEAN)){
            expect(token_type::BOOLEAN);
        }
        AST_p ret = nullptr;
        if(found(K_ALWAYS) || found(K_TRUE)){
            ret = make_ast(true, token());
        }else if(found(K_NEVER) || found(K_FALSE)){
            ret = make_ast(false, token());
        }else{
            ns_lexer::Token& tok = token();
            auto length = tok.to_string().length();
            auto op = tok.to_string().substr(length - 1, 1);
            auto num = tok.to_string().substr(0, length - 1);
            float fl;
            try {
                fl = std::stof(num);
            }catch (...){
                throw ParserException(error("Not a valid boolean"));
            }
            ret = make_ast(op, make_ast(fl, tok), token());
        }
        read();
        return ret;
    }

    AST_p Parser::number() {
        expect(grammar::token_type::NUMBER);
        AST_p ret = nullptr;
        try {
            ret = make_ast(std::stof(token().to_string()), token());
        }catch (...){
            throw ParserException(error("Not a valid number"));
        }
        read();
        return ret;
    }

    AST_p Parser::string() {
        expect(grammar::token_type::STRING);
        AST_p ret = nullptr;
        if(token().to_string()[token().to_string().length() - 1] == grammar::C_OPEN_BRACKET){ //Cstring
            ns_lexer::Token& tok = token();
            std::vector<AST_p> nodes = {make_ast(token().to_string(), tok)};
            int sindex = tok.get_index();
            int length = 0;
            while(token().to_string()[token().to_string().length() - 1] == grammar::C_OPEN_BRACKET){
                read();
                int next = csnext();
                if(next == -1){
                    throw ParserError(error("Complex string does not terminate correctly"));
                }
                push(index, next);
                AST_p val = find(&Parser::value);
                pop();
                nodes.push_back(val);
                expect(grammar::token_type::STRING);
                nodes.push_back(make_ast(token().to_string(), token()));
                length = (int) (token().get_index() - sindex + token().to_string().length());
            }
            ret = make_ast(std::move(nodes), tok, 0, length);
        }else{
            ret = make_ast(token().to_string(), token());
        }
        read();
        return ret;
    }

    AST_p Parser::identifier(){
        bool special = false;
        AST_p ret = nullptr;
        if(found(grammar::S_SPECIAL_SCOPE)){
            special = true;
            read();
        }
        expect(grammar::token_type::IDENTIFIER);
        if(!special){
            ret = make_ast(nullptr, token().to_string(), token());
        }else{
            ret = make_ast(grammar::S_SPECIAL_SCOPE, make_ast(nullptr, token().to_string(), token(), 3), token());
        }
        read();
        return ret;
    }

    AST_p Parser::list_element() {
        AST_p ret = nullptr, val = nullptr, prob = nullptr;
        ns_lexer::Token& tok = token();
        int oright = tok.get_index();
        val = find(&Parser::value);
        if(found(grammar::S_IF)){
            read();
            std::string boolean = token().to_string();
            if(boolean.back() != grammar::C_CARET && boolean.back() != grammar::C_PERCENTAGE){
                throw ParserException(error("Invalid boolean"));
            }
            oright = (int) (token().get_index() - oright + boolean.size());
            prob = find(&Parser::boolean);
            ret = make_ast(grammar::S_IF, val, prob, tok, 0, oright);
        }else{
            ret = val;
        }
        return ret;
    }

    AST_p Parser::list() {
        expect(grammar::S_OPEN_SQBRACKET);
        ns_lexer::Token& tok = token();
        read();
        std::vector<AST_p> list_elems;
        list_elems.push_back((AST_p) find(&Parser::list_element));
        while(found(grammar::S_COMMA)){
            read();
            list_elems.push_back((AST_p) find(&Parser::list_element));
        }
        expect(grammar::S_CLOSE_SQBRACKET);
        AST_p ret = make_ast(std::move(list_elems), ns_ast::node_type::NONE, tok, 0, token().get_index() - tok.get_index());
        read();
        return ret;
    }

    AST_p Parser::enumeration_name() {
        expect(grammar::token_type::IDENTIFIER);
        AST_p ret = make_ast(nullptr, token().to_string(), token());
        read();
        return ret;
    }

    AST_p Parser::enumeration() {
        expect(grammar::S_OPEN_BRACKET);
        ns_lexer::Token& tok = token();
        read();
        std::vector<std::string> enum_elems;
        enum_elems.push_back(((AST_p) find(&Parser::enumeration_name))->val.s);
        while(found(grammar::S_COMMA)){
            read();
            enum_elems.push_back(((AST_p) find(&Parser::enumeration_name))->val.s);
        }
        expect(grammar::S_CLOSE_BRACKET);
        AST_p ret = make_ast(std::move(enum_elems), tok, 0, token().get_index() - tok.get_index());
        read();
        return ret;
    }

    AST_p Parser::literal() {
        return find(&Parser::boolean) || find(&Parser::string) || find(&Parser::number);
    }

    AST_p Parser::list_expr_parens(){
        expect(grammar::S_OPEN_PARENTHESIS);
        read();
        AST_p ret = find(&Parser::list_expr);
        expect(grammar::S_CLOSE_PARENTHESIS);
        read();
        return ret;
    }

    AST_p Parser::list_expr_pick(){
        using namespace grammar;
        int op = firstof({S_PICK, S_EXPICK, S_UNBIASED_PICK, S_UNBIASED_EXPICK});
        if(op == -1){
            throw ParserException(error("Not a list expression"));
        }
        std::string oper = tokens[op].to_string();
        AST_p left, right;
        if(index == op){
            left = make_ast(1.0f, token());
            read();
        }else{
            push(index, op);
            left = find(&Parser::list_expr) || find(&Parser::list) || find(&Parser::identifier) || find(&Parser::number);
            pop();
            read();
        }
        try{
            right = find(&Parser::list_expr) || find(&Parser::list) || find(&Parser::identifier) || find(&Parser::number);
        }catch(ParserException& ex){
            right = make_ast(1.0f, token());
        }
        return make_ast(oper, left, right, tokens[op]); //TODO improve
    }

    AST_p Parser::list_expr_chancify(){
        AST_p iden, list;
        std::string op = grammar::S_SPECIAL_SCOPE;
        ns_lexer::Token& op_tok = token();
        iden = find(&Parser::identifier);
        expect(grammar::S_OPEN_BRACKET);
        read();
        ns_lexer::Token& tok = token();
        std::vector<AST_p> probs;
        AST_p curval = find(&Parser::boolean);
        if(curval->type != ns_ast::UNARY){
            throw ParserException(error("Invalid boolean"));
        }
        probs.push_back(curval);
        while(found(grammar::S_COMMA)){
            read();
            curval = find(&Parser::boolean);
            if(curval->type != ns_ast::UNARY){
                throw ParserException(error("Invalid boolean"));
            }
            probs.push_back(curval);
        }
        expect(grammar::S_CLOSE_BRACKET);
        list = make_ast(std::move(probs), ns_ast::BOOLEAN, tok);
        read();
        return make_ast(op, iden, list, op_tok); //TODO Improve
    }

    AST_p Parser::list_expr_index(){
        AST_p list, integer;
        int start = lastof({grammar::S_OPEN_SQBRACKET});
        if(start == -1 || start == index){
            throw ParserException(error("Not indexing"));
        }
        ns_lexer::Token& tok = tokens[start];
        push(index, start);
        list = find(&Parser::list_expr) || find(&Parser::list) || find(&Parser::identifier);
        pop();
        expect(grammar::S_OPEN_SQBRACKET);
        read();
        integer = find(&Parser::number);
        expect(grammar::S_CLOSE_SQBRACKET);
        read();
        return make_ast(grammar::S_SQBRACKETS, list, integer, tok);
    }


    AST_p Parser::list_expr() {
        return find(&Parser::list_expr_parens) ||
               find(&Parser::list_expr_index)  ||
               find(&Parser::list_expr_pick)   ||
               find(&Parser::list_expr_chancify);
    }

    AST_p Parser::boolean_expr_parens(){
        expect(grammar::S_OPEN_PARENTHESIS);
        read();
        AST_p ret = find(&Parser::boolean_expr);
        expect(grammar::S_CLOSE_PARENTHESIS);
        read();
        return ret;
    }

    AST_p Parser::boolean_expr_unary(){
        expect(grammar::K_NOT);
        ns_lexer::Token& tok = token();
        read();
        AST_p operand = find(&Parser::boolean_expr) || find(&Parser::boolean) || find(&Parser::identifier);
        return make_ast(tok.to_string(), operand, tok); //TODO Improve
    }

    AST_p Parser::boolean_expr_binary(){
        int op_pos = firstof({grammar::K_AND, grammar::K_OR});
        if(op_pos == -1){
            throw ParserException(error("Not AND/OR expression"));
        }
        AST_p left = nullptr, right = nullptr;
        std::string op = tokens[op_pos].to_string();
        push(index, op_pos);
        left = find(&Parser::boolean_expr) || find(&Parser::boolean) || find(&Parser::identifier);
        pop();
        expect(op);
        read();
        right = find(&Parser::boolean_expr) || find(&Parser::boolean) || find(&Parser::identifier);
        return make_ast(op, left, right, tokens[op_pos]); //TODO Improve
    }

    AST_p Parser::boolean_expr_comparison(){
        int op_pos = firstof({grammar::S_EQUALS, grammar::S_NOT_EQUALS});
        if(op_pos == -1){
            throw ParserException(error("Not =/~= expression"));
        }
        AST_p left = nullptr, right = nullptr;
        std::string op = tokens[op_pos].to_string();
        push(index, op_pos);
        left = find(&Parser::value);
        pop();
        expect(op);
        read();
        right = find(&Parser::value);
        return make_ast(op, left, right, tokens[op_pos]); //TODO Improve
    }


    AST_p Parser::boolean_expr() {
        return find(&Parser::boolean_expr_parens) ||
               find(&Parser::boolean_expr_unary)  ||
               find(&Parser::boolean_expr_binary)   ||
               find(&Parser::boolean_expr_comparison);
    }

    AST_p Parser::value() {
        AST_p ret = nullptr;
        if(found(grammar::S_OPEN_PARENTHESIS)){
            read();
            ret = find(&Parser::value);
            expect(grammar::S_CLOSE_PARENTHESIS);
            read();
        }else{
            ret = find(&Parser::boolean_expr) ||
                  find(&Parser::list_expr)    ||
                  find(&Parser::identifier)   ||
                  find(&Parser::list)         ||
                  find(&Parser::literal);
        }
        return ret;
    }

    AST_p Parser::if_expr() {
        expect(grammar::S_IF);
        ns_lexer::Token& tok = token();
        read();
        AST_p val = find(&Parser::value);
        return make_ast(grammar::S_IF, nullptr, val, tok); //TODO make better
    }

    AST_p Parser::expression() {
        AST_p ret = nullptr;
        if(found(grammar::S_OPEN_PARENTHESIS)){
            read();
            ret = find(&Parser::expression);
            expect(grammar::S_CLOSE_PARENTHESIS);
            read();
        }else{
            ret = find(&Parser::enumeration) || find(&Parser::value);
        }
        return ret;
    }

    AST_p Parser::import_statement() {
        expect(grammar::K_IMPORT);
        ns_lexer::Token& tok = token();
        read();
        AST_p what = nullptr, that = nullptr;
        ns_lexer::Token& stok = token();
        what = find(&Parser::identifier) || find(&Parser::string);
        if(what->type == ns_ast::UNARY || what->type == ns_ast::CSTRING){
            throw ParserException(error("Invalid import target"));
        }else if(what->type == ns_ast::VARIABLE && what->val.s.find(grammar::C_DOT) != std::string::npos){
            throw ParserException(error("Invalid import target"));
        }
        if(found(grammar::K_AS)){
            read();
            that = find(&Parser::identifier);
            if(that->type == ns_ast::UNARY){
                throw ParserException(error("Invalid import alias"));
            }else if(that->val.s.find(grammar::C_DOT) != std::string::npos){
                throw ParserException(error("Invalid import alias"));
            }
        }else{
            auto begin = what->val.s.find_first_not_of("\"");
            auto end   = what->val.s.find_last_not_of("\"");
            std::string as = what->val.s.substr(begin, end - begin + 1);
            that = make_ast(nullptr, as, tok);
        }
        return make_ast(grammar::K_IMPORT, make_ast(grammar::K_AS, what, that, stok), tok);
    }

    AST_p Parser::declaration_statement() {
        expect(grammar::token_subtype::KEYWORD_TYPENAME);
        ns_lexer::Token& type = token();
        read();
        AST_p assnmnt = find(&Parser::assignment_statement);
        assnmnt->val.bin.left = make_ast(type.to_string(), assnmnt->val.bin.left, type);
        return assnmnt;
    }

    AST_p Parser::assignment_statement() {
        AST_p what = nullptr, that = nullptr;
        what = find(&Parser::identifier);
        if(what->type == ns_ast::UNARY){
            throw ParserException(error("Invalid assignment target"));
        }
        expect(grammar::S_IS);
        ns_lexer::Token& tok = token();
        read();
        that = find(&Parser::expression);
        return make_ast(grammar::S_IS, what, that, tok);
    }

    AST_p Parser::keyword_statement() {
        expect(grammar::token_subtype::KEYWORD_BLOCKPROP);
        ns_lexer::Token& tok = token();
        read();
        AST_p val = find(&Parser::value);
        return make_ast(tok.to_string(), val, tok);
    }

    AST_p Parser::use_statement() {
        expect(grammar::K_USE);
        ns_lexer::Token& tok = token();
        read();
        AST_p what = nullptr, that = nullptr;
        ns_lexer::Token& stok = token();
        what = find(&Parser::identifier);
        if(what->type == ns_ast::UNARY){
            throw ParserException(error("Invalid use target"));
        }
        if(found(grammar::K_AS)){
            read();
            that = find(&Parser::identifier);
            if(that->type == ns_ast::UNARY){
                throw ParserException(error("Invalid use alias"));
            }else if(that->val.s.find(grammar::C_DOT) != std::string::npos){
                throw ParserException(error("Invalid use alias"));
            }
        }else{
            std::string as = what->val.s.substr(what->val.s.find_last_of(grammar::C_DOT) + 1);
            that = make_ast(nullptr, as, what->token);
        }
        return make_ast(grammar::K_USE, make_ast(grammar::K_AS, what, that, stok), tok);
    }

    AST_p Parser::repr_statement() {
        expect(grammar::token_subtype::KEYWORD_REPR);
        ns_lexer::Token& tok = token();
        read();
        AST_p ret = nullptr;
        if(tok.to_string() == grammar::K_REPR) {
            AST_p expr = find(&Parser::expression), if_exp = nullptr;
            if (found(grammar::S_IF)) {
                if_exp = find(&Parser::if_expr);
                if_exp->val.bin.left = expr;
                expr = if_exp;
                next_was_cond_repr = true;
            }
            ret = make_ast(grammar::K_REPR, expr, tok);
        }else if(tok.to_string() == grammar::K_ELSE){
            if(!was_cond_repr) {
                throw ParserException(error("Orphan else"));
            }
            AST_p repr = find(&Parser::repr_statement);
            ret = make_ast(grammar::K_ELSE, repr, tok);
        }else{
            AST_p list = find(&Parser::list_expr) || find(&Parser::list) || find(&Parser::identifier),
                  if_exp = nullptr;
            list = make_ast(grammar::S_PICK, list, make_ast(1.0f, tok), tok);
            if (found(grammar::S_IF)) {
                if_exp = find(&Parser::if_expr);
                if_exp->val.bin.left = list;
                list = if_exp;
                next_was_cond_repr = true;
            }
            ret = make_ast(grammar::K_OPTS, list, tok);
        }
        return ret;
    }

    AST_p Parser::block_statement() {
        expect(grammar::token_subtype::KEYWORD_BLOCKNAME);
        ns_lexer::Token& tok = token();
        std::string bts = tok.to_string();
        ns_ast::block_type bt;
        if(bts == grammar::K_LOCATION){
            bt = ns_ast::LOCATION;
        }else if(bts == grammar::K_EVENT){
            bt = ns_ast::EVENT;
        }else if(bts == grammar::K_CONTAINS){
            bt = ns_ast::CONTAINS;
        }else{
            throw ParserException(error("Illegal block type"));
        }
        read();
        AST_p name = nullptr, app = nullptr;
        std::vector<AST_p> stmts;
        name = find(&Parser::identifier);
        if(name->type == ns_ast::UNARY){
            throw ParserException(error("Invalid block name"));
        }else if(name->val.s.find(grammar::C_DOT) != std::string::npos){
            throw ParserException(error("Invalid block name"));
        }
        if(found(grammar::S_OPEN_PARENTHESIS)){
            read();
            app = find(&Parser::boolean);
            expect(grammar::S_CLOSE_PARENTHESIS);
            read();
            app = make_ast(grammar::K_APPEAR, app, app->token);
        }
        if(app){
            stmts.push_back(app);
        }
        if(found(grammar::S_COLON)){
            // It has more shit
            pop(); //Limits make no sense anymore
            read();
            expect(grammar::token_type::NEWLINE);
            read();
            int startindex = index;
            int bindentation = whitespace();
            int indentation = bindentation;
            while(indentation == bindentation){
                if(!found(grammar::token_type::NEWLINE)){
                    stmts.push_back((AST_p) find(&Parser::statement));
                }
                read();
                startindex = index;
                indentation = whitespace();
            }
            index = startindex - 1;
            push(ll, rl);
        }
        return make_ast(bt, name->val.s, std::move(stmts), tok);
    }

    AST_p Parser::generate_statement() {
        expect(grammar::K_GENERATE);
        ns_lexer::Token& tok = token();
        read();
        AST_p iden = find(&Parser::identifier);
        if(iden->type == ns_ast::UNARY){
            throw ParserException(error("Invalid generate target"));
        }
        return make_ast(grammar::K_GENERATE, iden, tok);
    }

    AST_p Parser::statement() {
        push(index, line_end());
        AST_p ret =
               find(&Parser::declaration_statement) ||
               find(&Parser::assignment_statement)  ||
               find(&Parser::keyword_statement)     ||
               find(&Parser::use_statement)         ||
               find(&Parser::repr_statement)        ||
               find(&Parser::block_statement)       ||
               find(&Parser::import_statement)      ||
               find(&Parser::generate_statement)    ||
               find(&Parser::expression);
        pop();
        was_cond_repr = next_was_cond_repr;
        next_was_cond_repr = false;
        return ret;
    }

    bool Parser::found(std::string content) {
        return tokens[index].to_string() == content;
    }

    bool Parser::found(grammar::token_type type) {
        return tokens[index].get_type() == type;
    }

    bool Parser::found(grammar::token_subtype subtype) {
        return grammar::is(tokens[index].to_string(), subtype);
    }

    void Parser::expect(std::string content) {
        if(!found(content)){
            throw ParserException(error("Expected " + content + ", got " + tokens[index].to_string() + " instead"));
        }
    }

    void Parser::expect(grammar::token_type type) {
        if(!found(type)){
            std::stringstream ss;
            ss << "Expected " <<  grammar::token_type_names[(int) type] << ", got ";
            ss << grammar::token_type_names[(int) tokens[index].get_type()] << " instead";
            throw ParserException(error(ss.str()));
        }
    }

    void Parser::expect(grammar::token_subtype subtype) {
        if(!found(subtype)){
            throw ParserException(error("Expected subtype, did not get ¯\\_(ツ)_/¯")); //TODO Fix this
        }
    }

    int Parser::firstof(std::vector<std::string> elements) {
        int depth = 0;
        std::string opener;
        std::string closer;
        for(int i = index; i < rl; i++){
            ns_lexer::Token& tok = tokens[i];
            if(depth < 1 || (depth == 1 && (tok.to_string() == closer))) {
                for (auto &element : elements) {
                    if (tokens[i].to_string() == element) {
                        return i;
                    }
                }
            }
            if(depth == 0 && grammar::is(tok.to_string(), grammar::token_subtype::SYMBOL_OPENER)){
                depth++;
                opener = tok.to_string();
                if(opener == grammar::S_OPEN_PARENTHESIS){
                    closer = grammar::S_CLOSE_PARENTHESIS;
                }else if(opener == grammar::S_OPEN_BRACKET){
                    closer = grammar::S_CLOSE_BRACKET;
                }else if(opener == grammar::S_OPEN_SQBRACKET){
                    closer = grammar::S_CLOSE_SQBRACKET;
                }
            }else if(depth > 0 && tok.to_string() == opener){
                depth++;
            }else if(tok.to_string() == closer){
                depth--;
            }
        }
        return -1;
    }

    int Parser::lastof(std::vector<std::string> elements) {
        int depth = 0;
        std::string opener;
        std::string closer;
        for (int i = rl - 1; i >= index; i--) {
            ns_lexer::Token& tok = tokens[i];
            if(depth < 1 || (depth == 1 && (tok.to_string() == closer))) {
                for (auto &element : elements) {
                    if (tokens[i].to_string() == element) {
                        return i;
                    }
                }
            }
            if(depth == 0 && grammar::is(tok.to_string(), grammar::token_subtype::SYMBOL_CLOSER)){
                depth++;
                opener = tok.to_string();
                if(opener == grammar::S_CLOSE_PARENTHESIS){
                    closer = grammar::S_OPEN_PARENTHESIS;
                }else if(opener == grammar::S_CLOSE_BRACKET){
                    closer = grammar::S_OPEN_BRACKET;
                }else if(opener == grammar::S_CLOSE_SQBRACKET){
                    closer = grammar::S_OPEN_SQBRACKET;
                }
            }else if(depth > 0 && tok.to_string() == opener){
                depth++;
            }else if(tok.to_string() == closer){
                depth--;
            }
        }
        return -1;
    }

    int Parser::next(){
        for(int i = index; i < rl; i++){
            if(tokens[i].get_type() != grammar::token_type::WHITESPACE &&
               tokens[i].get_type() != grammar::token_type::NEWLINE &&
               tokens[i].get_type() != grammar::token_type::COMMENT &&
               tokens[i].get_type() != grammar::token_type::NONE){
                return i;
            }
        }
        return -1;
    }

    int Parser::csnext(){
        int depth = 1;
        for(int i = index; i < rl; i++){
            if(tokens[i].get_type() == grammar::token_type::STRING){
                std::string content = tokens[i].to_string();
                if(content[0] == grammar::C_CLOSE_BRACKET){
                    depth--;
                }
                if(depth == 0){
                    return i;
                }
                if(content[content.length()-1] == grammar::C_OPEN_BRACKET){
                    depth++;
                }
            }
        }
        return -1;
    }

    int Parser::line_end(){
        for(int i = index; i < rl; i++){
            if(tokens[i].get_type() == grammar::token_type::NEWLINE ||
               tokens[i].get_type() == grammar::token_type::END){
                return i;
            }
        }
        return -1;
    }

    void Parser::push(int ll, int rl) {
        std::pair<int, int> tp = std::make_pair(Parser::ll, Parser::rl);
        if(ll < Parser::ll){
            std::stringstream ss;
            ss << "[PARSER ERROR] Putting left limit before current limit (" << ll << " < " << Parser::ll << ")";
            throw ParserError(color::ansi(color::FG_BRED) + ss.str() + color::ansi(color::ALL_RESET));
        }
        if(rl > Parser::rl){
            std::stringstream ss;
            ss << "[PARSER ERROR] Putting right limit after current limit (" << rl << " > " << Parser::rl << ")";
            throw ParserError(color::ansi(color::FG_BRED) + ss.str() + color::ansi(color::ALL_RESET));
        }
        limits.push(tp);
        Parser::ll = ll;
        Parser::rl = rl;
    };

    void Parser::push(std::pair<int, int> limit) {
        push(limit.first, limit.second);
    }

    std::pair<int, int> Parser::pop() {
        if(limits.empty()){
            throw ParserError(color::ansi(color::FG_BRED) +
                              "[PARSER ERROR] Popping from state stack when there is nothing to pop" +
                              color::ansi(color::ALL_RESET));
        }
        auto ret = limits.top();
        ll = ret.first;
        rl = ret.second;
        limits.pop();
        return ret;
    }

    std::string Parser::error(std::string message){
        using namespace color;
        ns_lexer::Token& tok = tokens[index];
        int col = tok.get_col(), line = tok.get_line();
        auto length = tok.to_string().length();
        std::stringstream ss;
        ss << ansi(FG_BRED) << "[PARSER EXCEPTION] Line: " << line << " Col: " << col << ": " << message << std::endl;
        ss << tok.get_full_line() << std::endl;
        ss << std::setw(col + length - 1) << std::string(length, '^') << ansi(ALL_RESET);
        return ss.str();
    }

}