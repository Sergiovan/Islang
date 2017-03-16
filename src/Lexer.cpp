//
// Created by Sergiovan on 04-Mar-17.
//

#include <string>
#include <vector>

#include "Color.h"
#include "Grammar.h"
#include "Token.h"
#include "Lexer.h"
#include "Scanner.h"
#include "TokenException.h"

using namespace ns_lexer;

Lexer::Lexer() : new_line(false), error(false), nesting(0) {}

std::vector<Token> Lexer::read_file(std::string filename) {
    ns_scanner::Scanner sc;
    sc.open_file(filename);
    return read(sc);
}

std::vector<Token> Lexer::read_string(std::string string) {
    ns_scanner::Scanner sc;
    sc.open_string(string);
    return read(sc);
}

bool Lexer::okay() {
    return !error;
}

Token Lexer::next(ns_scanner::Scanner& sc) {
    typedef grammar::token_type tt;
    typedef grammar::token_subtype ts;
    typedef grammar::character_type ct;
    using namespace grammar;
    ns_scanner::Character c = sc.get();
    Token current(c);

    if(is(c, ct::WHITESPACE) && c != C_CARRIAGE_RETURN && new_line){ //Whitespace before content
        current.set_type(tt::WHITESPACE);
        return current;
    }else if(is(c, ct::WHITESPACE)){
        sc.discard_while({C_WHITESPACE, C_TAB, C_CARRIAGE_RETURN}); //Discard whitespaces
        c = sc.get();
        current = Token(c); //First non-whitespace character
    }

    if(is(c, ct::END_OF_FILE)){ //The end, my friend
        current.set_type(tt::END);
        return current;
    }

    if(is(c, ct::NEWLINE) || c == '\r'){ //A newline, discard all new lines tho
        new_line = true;
        sc.discard_while({C_NEWLINE, C_CARRIAGE_RETURN, C_CARRIAGE_RETURN});
        current = current + C_NEWLINE;
        current.set_type(tt::NEWLINE);
        return current;
    }

    if(c == C_HASH){ //Single line comment
        current.set_type(tt::COMMENT);
        char n = sc.peek();
        while(n != C_NEWLINE && n != EOF){
            if(n != C_CARRIAGE_RETURN){
                current = current + n;
            }
            c = sc.get();
            n = sc.peek();
        }
        return current;
    }

    if(c == '/' && sc.peek() == '*'){ //Kamments!
        current.set_type(tt::COMMENT);
        int nesting = 1;
        while(nesting > 0){
            c = sc.get();
            if(c == '*' && sc.peek() == '/'){
                nesting--;
                current = current + c;
                c = sc.get();
            }else if(c == '/' && sc.peek() == '*'){
                nesting++;
                current = current + c;
                c = sc.get();
            }
            current = current + c;
        }
        return current;
    }

    new_line = false; //At this point anything would break the new linelyness

    if(is(c, ct::DIGIT) && !(c == '-' && sc.peek() == '>')){ //Numbers and boolean literals
        current.set_type(tt::NUMBER);
        bool done = false;
        bool negative = c == C_MINUS;
        char n = sc.peek();
        while(!done){
            if(what(n) == ct::DIGIT || n == C_DOT){
                current = current + n;
            }else if(what(n) == ct::SYMBOL_BOOLEAN){ //Actually a boolean
                current.set_type(tt::BOOLEAN);
                current = current + n;
                if(negative){
                    current.die("Negative probabilities not allowed");
                }
                done = true;
            }else if(is(n, ct::LETTER) || what(n) == ct::SYMBOL_IDENTIFIER){
                current = current + n;
                current.die("Unexpected symbol near number");
            }else{ //Number ends
                break; //Since we found a space or whatever, don't consume it
            }
            sc.get();
            n = sc.peek();
        }
        return current;
    }

    if(c == C_QUOTE || (c == C_CLOSE_BRACKET && nesting > 0)){ //String
        if(c == C_CLOSE_BRACKET){
            nesting--;
        }
        current.set_type(tt::STRING);
        bool done = false;
        c = sc.get();
        while(!done){
            if(c == '\\'){
                c = sc.get();
                if(c == 'n'){ //TODO Make a map?
                    current = current + C_NEWLINE;
                }else if(c == 't'){
                    current = current + C_TAB;
                }else{
                    current = current + c;
                }
            }else if(c == C_QUOTE){
                current = current + c;
                break;
            }else if(c == C_OPEN_BRACKET){
                current = current + c;
                nesting++;
                break;
            }else if(is(c, ct::NEWLINE)){
                ; //Ignored
            }else if(c == EOF){ //K
                current.die("Unexpected end of string");
            }else{
                current = current + c;
            }
            c = sc.get();
        }
        return current;
    }

    if(!is(c, ct::LETTER) && c != C_UNDERSCORE){ //Symbols!
        current.set_type(tt::SYMBOL);
        std::string lookahead = {c, sc.peek()};
        if(is(lookahead, ts::SYMBOL_STANDALONE)){
            current = current + sc.get();
        }else if(is({c}, ts::SYMBOL_STANDALONE)){
            ; //Nothing
        }else{
            current.die("Unexpected character");
        }
        return current;
    }

    if(is(c, ct::LETTER) || c == C_UNDERSCORE){
        char n = sc.peek();
        while(what(n) == ct::SYMBOL_IDENTIFIER || is(n, ct::LETTER) || is(n, ct::DIGIT)){
            current = current + n;
            c = sc.get();
            n = sc.peek();
        }

        if(is(current, grammar::token_subtype::KEYWORD_BOOLEAN)){
            current.set_type(tt::BOOLEAN);
        }else if(is(current, tt::KEYWORD)){
            current.set_type(tt::KEYWORD);
        }else{
            current.set_type(tt::IDENTIFIER);
        }

        return current;

    }

    current.die("Unexpected character");
    throw std::exception();
}

std::vector<Token> Lexer::read(ns_scanner::Scanner& sc) {
    std::vector<Token> ret;
    while(!sc.done()){
        try {
            ret.push_back(next(sc));
        }catch(ns_lexer::TokenException& ex){
            std::cerr << ex.what() << std::endl;
            error = true;
        }
    }

    return ret;
}
