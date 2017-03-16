//
// Created by Sergiovan on 04-Mar-17.
//

#include <istream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <c++/6.3.0/cassert>

#include "Color.h"
#include "Grammar.h"
#include "Token.h"
#include "TokenException.h"


namespace ns_lexer {

    Token::Token() : content(""), file(nullptr), string(nullptr), is_file(false), index(-1),
                     line(-1), col(-1), line_start(-1), type(grammar::token_type::NONE){

    }

    Token::Token(ns_scanner::Character c)
            : content(1, c.c), file(c.file), string(c.string), is_file(c.is_file),
              index(c.index), line(c.line), col(c.col), line_start(c.line_start),
              type(grammar::token_type::NONE) {
    }

    Token::operator std::string() const {
        return to_string();
    }

    Token Token::operator+(char other) const {
        Token ret = *this;
        ret.content += other;
        return ret;
    }

    Token Token::operator+(ns_scanner::Character &other) const {
        Token ret = *this;
        return (ret + (char) other);
    }

    std::string Token::to_string() const {
        return content;
    }

    std::string Token::print() const {
        if(index < 0){
            return "";
        }
        using namespace color;
        std::stringstream ss;
        ss << "[" << ansi(FG_BCYAN) << std::setw(10) << grammar::token_type_names[(int) type] << ansi(ALL_RESET)
           << "] ";
        ss << std::setw(6) << index << " " << std::setw(6) << line << " " << std::setw(6) << col << "  ";
        std::string content = Token::content;
        if (type == grammar::token_type::NEWLINE) {
            content = ansi(FG_BBLUE) + "NEWLINE" + ansi(ALL_RESET);
        } else if (type == grammar::token_type::END) {
            content = ansi(FG_BBLUE) + "END OF FILE" + ansi(ALL_RESET);
        } else if (type == grammar::token_type::WHITESPACE) {
            content = ansi(FG_BBLUE) + "WHITESPACE" + ansi(ALL_RESET);
        } else if (content.find('\n') != std::string::npos || content.find('\r') != std::string::npos) {
            content = ansi(FG_BRED) + "(TOO LONG)" + ansi(ALL_RESET);
        }

        ss << '|' << content << '|';
        return ss.str();
    }

    std::string Token::get_full_line() const{
        if(index < 0){
            return "";
        }
        std::string errline;
        if (is_file) {
            long long int p_index = file->tellg();
            file->clear();
            file->seekg(line_start, std::ios_base::beg);
            std::getline(*file, errline);
            file->seekg(p_index, std::ios_base::beg);
        } else {
            long long int p_index = string->tellg();
            string->clear();
            string->seekg(line_start, std::ios_base::beg);
            std::getline(*string, errline);
            string->seekg(p_index, std::ios_base::beg);
        }
        return errline;
    }

    std::string Token::error(std::string message) const {
        if(index < 0){
            return "";
        }
        using namespace color;

        //TODO Line length
        std::stringstream ss;
        ss << ansi(FG_BRED) << "[TOKEN ERROR] Line " << line << ", Column " << col << ": " << message.c_str()
           << std::endl;
        ss << get_full_line() << std::endl;
        ss << get_full_line_pointer() << ansi(ALL_RESET);
        return ss.str();
    }

    std::string Token::get_full_line_pointer() const {
        if(index < 0){
            return "";
        }
        std::stringstream ss;
        ss << std::setw(col + content.length() - 1) << std::string(content.length(), '^');
        return ss.str();
    }

    void Token::die(std::string message) const {
        throw TokenException(error(message));
    }

    void Token::set_type(grammar::token_type new_type) {
        type = new_type;
    }

    grammar::token_type Token::get_type() const {
        return type;
    }

    int Token::get_index() const {
        return index;
    }

    int Token::get_line() const {
        return line;
    }

    int Token::get_col() const {
        return col;
    }

    const std::string Token::table_header = " Token Type   Index   Line    Col  Content";

}