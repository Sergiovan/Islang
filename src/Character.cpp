//
// Created by Sergiovan on 04-Mar-17.
//

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include "Character.h"

using namespace ns_scanner;

Character::operator char() const {
    return c;
}

std::string Character::to_string() const {
    std::stringstream ss;
    ss << std::setw(6) << index << std::setw(6) << line << std::setw(6) << col << std::setw(11);
    auto it = character_names.find(c);
    if(it != Character::character_names.end()){
        ss << " " << " " << it->second; //Not an error
    }else{
        ss << c;
    }
    return ss.str();
}

const std::string Character::table_header = " Index  Line   Col  Character (Name)";
const std::map<char, std::string> Character::character_names = {
    {'\r', "CARRIAGE RETURN"},
    {'\n', "NEWLINE"},
    {' ',  "SPACE"},
    {EOF,  "END OF FILE"},
    {'\t', "TAB"},
    {'\0', "NULL TERMINATOR"} //Not an error
};