//
// Created by Sergiovan on 05-Mar-17.
//

#include "Grammar.h"

namespace grammar {
    typedef token_type tt;
    typedef token_subtype ts;
    const std::map<std::string, std::pair<tt, std::set<ts>>> string_to_type_map = {
        {S_UNBIASED_PICK,     {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_UNBIASED_EXPICK,   {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_NOT_EQUALS,        {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_IS,                {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_SPECIAL_SCOPE,     {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_SQBRACKETS,        {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_DOT,               {tt::SYMBOL, {ts::SYMBOL_NUMBER, ts::SYMBOL_IDENTIFIER}}},
        {S_UNDERSCORE,        {tt::SYMBOL, {ts::SYMBOL_IDENTIFIER}}},
        {S_BANG,              {tt::SYMBOL, {ts::SYMBOL_IDENTIFIER}}},
        {S_QUESTION,          {tt::SYMBOL, {ts::SYMBOL_IDENTIFIER}}},
        {S_PERCENTAGE,        {tt::SYMBOL, {ts::SYMBOL_BOOLEAN}}},
        {S_CARET,             {tt::SYMBOL, {ts::SYMBOL_BOOLEAN}}},
        {S_PICK,              {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_EXPICK,            {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_IF,                {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_EQUALS,            {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_OPEN_PARENTHESIS,  {tt::SYMBOL, {ts::SYMBOL_STANDALONE, ts::SYMBOL_PARENTHESIS, ts::SYMBOL_OPENER}}},
        {S_CLOSE_PARENTHESIS, {tt::SYMBOL, {ts::SYMBOL_STANDALONE, ts::SYMBOL_PARENTHESIS, ts::SYMBOL_CLOSER}}},
        {S_OPEN_BRACKET,      {tt::SYMBOL, {ts::SYMBOL_STANDALONE, ts::SYMBOL_BRACKET, ts::SYMBOL_OPENER}}},
        {S_CLOSE_BRACKET,     {tt::SYMBOL, {ts::SYMBOL_STANDALONE, ts::SYMBOL_BRACKET, ts::SYMBOL_CLOSER}}},
        {S_OPEN_SQBRACKET,    {tt::SYMBOL, {ts::SYMBOL_STANDALONE, ts::SYMBOL_SQBRACKET, ts::SYMBOL_OPENER}}},
        {S_CLOSE_SQBRACKET,   {tt::SYMBOL, {ts::SYMBOL_STANDALONE, ts::SYMBOL_SQBRACKET, ts::SYMBOL_CLOSER}}},
        {S_COMMA,             {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_COLON,             {tt::SYMBOL, {ts::SYMBOL_STANDALONE}}},
        {S_DOLLAR,            {tt::SYMBOL, {}}},
        {K_FALSE,             {{tt::KEYWORD}, {ts::KEYWORD_BOOLEAN}}},
        {K_TRUE,              {{tt::KEYWORD}, {ts::KEYWORD_BOOLEAN}}},
        {K_NEVER,             {{tt::KEYWORD}, {ts::KEYWORD_BOOLEAN}}},
        {K_ALWAYS,            {{tt::KEYWORD}, {ts::KEYWORD_BOOLEAN}}},
        {K_AND,               {{tt::KEYWORD}, {ts::KEYWORD_OPERATOR}}},
        {K_OR,                {{tt::KEYWORD}, {ts::KEYWORD_OPERATOR}}},
        {K_NOT,               {{tt::KEYWORD}, {ts::KEYWORD_OPERATOR}}},
        {K_BOOL,              {{tt::KEYWORD}, {ts::KEYWORD_TYPENAME}}},
        {K_ENUM,              {{tt::KEYWORD}, {ts::KEYWORD_TYPENAME}}},
        {K_NUM,               {{tt::KEYWORD}, {ts::KEYWORD_TYPENAME}}},
        {K_LIST,              {{tt::KEYWORD}, {ts::KEYWORD_TYPENAME}}},
        {K_STR,               {{tt::KEYWORD}, {ts::KEYWORD_TYPENAME}}},
        {K_VAL,               {{tt::KEYWORD}, {ts::KEYWORD_TYPENAME}}},
        {K_CONTAINS,          {{tt::KEYWORD}, {ts::KEYWORD_BLOCKNAME}}},
        {K_EVENT,             {{tt::KEYWORD}, {ts::KEYWORD_BLOCKNAME}}},
        {K_LOCATION,          {{tt::KEYWORD}, {ts::KEYWORD_BLOCKNAME}}},
        {K_APPEAR,            {{tt::KEYWORD}, {ts::KEYWORD_BLOCKPROP}}},
        {K_AS,                {{tt::KEYWORD}, {}}},
        {K_COLOR,             {{tt::KEYWORD}, {ts::KEYWORD_BLOCKPROP}}},
        {K_GENERATE,          {{tt::KEYWORD}, {}}},
        {K_IMPORT,            {{tt::KEYWORD}, {}}},
        {K_NAME,              {{tt::KEYWORD}, {ts::KEYWORD_BLOCKPROP}}},
        {K_OPTS,              {{tt::KEYWORD}, {ts::KEYWORD_REPR}}},
        {K_REPR,              {{tt::KEYWORD}, {ts::KEYWORD_REPR}}},
        {K_ELSE,              {{tt::KEYWORD}, {ts::KEYWORD_REPR}}},
        {K_USE,               {{tt::KEYWORD}, {}}},
        {K_DUMMY,             {{tt::KEYWORD}, {}}},
    };
}

const char * grammar::token_type_names[] = {
        "Symbol",
        "Keyword",
        "String",
        "Boolean",
        "Number",
        "Identifier",
        "Whitespace",
        "Newline",
        "Comment",
        "EOF",
        "None"
};

const char* grammar::character_type_names[] = {
    "Digit",
    "Symbol",
    "Number symbol",
    "Boolean symbol",
    "Identifier legal symbol",
    "Letter",
    "Blank",
    "Whitespace",
    "Newline"
};

const char* grammar::block_type_names[] = {
    "Location",
    "Event",
    "Contains",
    "Repr",
    "Program",
    "None"
};

grammar::character_type grammar::what(char c){
    typedef grammar::character_type ct;
    switch(c){
        case C_PICK:
        case C_EXPICK:
        case C_IF:
        case C_EQUALS:
        case C_OPEN_PARENTHESIS:
        case C_CLOSE_PARENTHESIS:
        case C_OPEN_BRACKET:
        case C_CLOSE_BRACKET:
        case C_OPEN_SQBRACKET:
        case C_CLOSE_SQBRACKET:
        case C_COMMA:
        case C_COLON:
        case C_QUOTE:
            return ct::SYMBOL;
        case C_DOT:
        case C_UNDERSCORE:
        case C_BANG:
        case C_QUESTION:
            return ct::SYMBOL_IDENTIFIER;
        case C_MINUS:
            return ct::SYMBOL_NUMBER;
        case C_PERCENTAGE:
        case C_CARET:
            return ct::SYMBOL_BOOLEAN;
        case C_WHITESPACE:
        case C_TAB:
        case C_CARRIAGE_RETURN:
            return ct::WHITESPACE;
        case C_NEWLINE:
            return ct::NEWLINE;
        case EOF:
            return ct::END_OF_FILE;
        default:
            if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c > 127){
                return ct::LETTER;
            }else if(c >= '0' && c <= '9'){
                return ct::DIGIT;
            }else{
                return ct::NONE;
            }
    }
}

bool grammar::is(char c, grammar::character_type type){
    typedef grammar::character_type ct;
    character_type ctype = grammar::what(c);
    if(ctype == type){
        return true;
    }else{
        switch(type){
            case ct::DIGIT:
                return ctype == ct::SYMBOL_NUMBER;
            case ct::SYMBOL:
                return ctype == ct::SYMBOL_NUMBER || ctype == ct::SYMBOL_IDENTIFIER || ctype == ct::SYMBOL_BOOLEAN;
            case ct::SYMBOL_NUMBER:
            case ct::SYMBOL_BOOLEAN:
            case ct::SYMBOL_IDENTIFIER:
                return ctype == ct::SYMBOL;
            case ct::LETTER:
                return ctype == ct::SYMBOL_IDENTIFIER;
            case ct::BLANK:
                return ctype == ct::WHITESPACE || ctype == ct::NEWLINE || ctype == ct::END_OF_FILE;
            case ct::WHITESPACE:
            case ct::NEWLINE:
            case ct::END_OF_FILE:
                return ctype == ct::BLANK;
            case ct::NONE:
                return false;
        }
    }
    return false; // How would this even happen?
}

bool grammar::is(std::string str, grammar::token_type tt){
    auto it = string_to_type_map.find(str);
    return it != string_to_type_map.end() && it->second.first == tt;
}

bool grammar::is(std::string str, grammar::token_subtype ts){
    auto it = string_to_type_map.find(str);
    return it != string_to_type_map.end() && it->second.second.count(ts) > 0;
}