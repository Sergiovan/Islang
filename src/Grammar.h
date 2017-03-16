//
// Created by Sergiovan on 05-Mar-17.
//

#ifndef ISLANG_GRAMMAR_H
#define ISLANG_GRAMMAR_H

#include <map>
#include <string>
#include <set>
#include <utility>

namespace grammar {

    constexpr char C_PICK               = '*';
    constexpr char C_EXPICK             = '/';
    constexpr char C_IF                 = '~';
    constexpr char C_EQUALS             = '=';
    constexpr char C_OPEN_PARENTHESIS   = '(';
    constexpr char C_CLOSE_PARENTHESIS  = ')';
    constexpr char C_OPEN_BRACKET       = '{';
    constexpr char C_CLOSE_BRACKET      = '}';
    constexpr char C_OPEN_SQBRACKET     = '[';
    constexpr char C_CLOSE_SQBRACKET    = ']';
    constexpr char C_COMMA              = ',';
    constexpr char C_COLON              = ':';
    constexpr char C_DOT                = '.';
    constexpr char C_UNDERSCORE         = '_';
    constexpr char C_BANG               = '!';
    constexpr char C_QUESTION           = '?';
    constexpr char C_MINUS              = '-';
    constexpr char C_PERCENTAGE         = '%';
    constexpr char C_CARET              = '^';
    constexpr char C_HASH               = '#';
    constexpr char C_QUOTE              = '"';
    constexpr char C_DOLLAR             = '$';

    constexpr char C_WHITESPACE         = ' ';
    constexpr char C_TAB                = '\t';
    constexpr char C_NEWLINE            = '\n';
    constexpr char C_CARRIAGE_RETURN    = '\r';

    constexpr auto S_UNBIASED_PICK       = "**";
    constexpr auto S_UNBIASED_EXPICK     = "//";
    constexpr auto S_NOT_EQUALS          = "~=";
    constexpr auto S_IS                  = "->";
    constexpr auto S_SPECIAL_SCOPE       = "::";
    constexpr auto S_SQBRACKETS          = "[]";
    constexpr auto S_DOT                 = ".";
    constexpr auto S_UNDERSCORE          = "_";
    constexpr auto S_BANG                = "!";
    constexpr auto S_QUESTION            = "?";
    constexpr auto S_PERCENTAGE          = "%";
    constexpr auto S_CARET               = "^";
    constexpr auto S_PICK                = "*";
    constexpr auto S_EXPICK              = "/";
    constexpr auto S_IF                  = "~";
    constexpr auto S_EQUALS              = "=";
    constexpr auto S_OPEN_PARENTHESIS    = "(";
    constexpr auto S_CLOSE_PARENTHESIS   = ")";
    constexpr auto S_OPEN_BRACKET        = "{";
    constexpr auto S_CLOSE_BRACKET       = "}";
    constexpr auto S_OPEN_SQBRACKET      = "[";
    constexpr auto S_CLOSE_SQBRACKET     = "]";
    constexpr auto S_COMMA               = ",";
    constexpr auto S_COLON               = ":";
    constexpr auto S_DOLLAR              = "$";

    constexpr auto K_FALSE               = "false";
    constexpr auto K_TRUE                = "true";
    constexpr auto K_NEVER               = "never";
    constexpr auto K_ALWAYS              = "always";
    constexpr auto K_AND                 = "and";
    constexpr auto K_OR                  = "or";
    constexpr auto K_NOT                 = "not";
    constexpr auto K_BOOL                = "bool";
    constexpr auto K_ENUM                = "enum";
    constexpr auto K_NUM                 = "num";
    constexpr auto K_LIST                = "list";
    constexpr auto K_STR                 = "str";
    constexpr auto K_VAL                 = "val";
    constexpr auto K_CONTAINS            = "contains";
    constexpr auto K_EVENT               = "event";
    constexpr auto K_LOCATION            = "location";
    constexpr auto K_APPEAR              = "appear";
    constexpr auto K_AS                  = "as";
    constexpr auto K_COLOR               = "color";
    constexpr auto K_ELSE                = "else";
    constexpr auto K_GENERATE            = "generate";
    constexpr auto K_IMPORT              = "import";
    constexpr auto K_NAME                = "name";
    constexpr auto K_OPTS                = "opts";
    constexpr auto K_REPR                = "repr";
    constexpr auto K_USE                 = "use";
    constexpr auto K_DUMMY               = "ðŸ˜¢";



    enum class character_type : int {
        DIGIT = 0,
        SYMBOL,
        SYMBOL_NUMBER,
        SYMBOL_BOOLEAN,
        SYMBOL_IDENTIFIER,
        LETTER,
        BLANK,
        WHITESPACE,
        NEWLINE,
        END_OF_FILE,
        NONE
    };

    enum class token_type : int {
        SYMBOL = 0, // * ** / // ( ) { } [ ] <- = ~=...
        KEYWORD,    // and or not location event contains...
        STRING,     // "This is a string"
        BOOLEAN,    // true false always never 10% 66^
        NUMBER,     // 1 -1 1.1
        IDENTIFIER, // anything that is clearly different
        WHITESPACE, //
        NEWLINE,    //
        COMMENT,    // #Comment /* Comment too */
        END,        // EOF
        NONE
    };

    enum class token_subtype : int{
        SYMBOL_STANDALONE = 0,
        SYMBOL_BOOLEAN,
        SYMBOL_NUMBER,
        SYMBOL_STRING,
        SYMBOL_IDENTIFIER,
        SYMBOL_OPENER,
        SYMBOL_CLOSER,
        SYMBOL_PARENTHESIS,
        SYMBOL_BRACKET,
        SYMBOL_SQBRACKET,
        KEYWORD_OPERATOR,
        KEYWORD_BOOLEAN,
        KEYWORD_TYPENAME,
        KEYWORD_BLOCKNAME,
        KEYWORD_BLOCKPROP,
        KEYWORD_REPR        //Etc
    };

    extern const std::map<std::string, std::pair<grammar::token_type, std::set<grammar::token_subtype>>> string_to_type_map;

    extern const char* token_type_names[];
    extern const char* character_type_names[];

    extern const char* block_type_names[];

    grammar::character_type what(char c);
    bool           is(char c, grammar::character_type type);
    bool           is(std::string str, grammar::token_type type);
    bool           is(std::string str, grammar::token_subtype type);

};


#endif //ISLANG_GRAMMAR_H
