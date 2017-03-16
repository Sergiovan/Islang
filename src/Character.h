//
// Created by Sergiovan on 04-Mar-17.
//

#ifndef ISLANG_CHARACTER_H
#define ISLANG_CHARACTER_H

#include <iostream>
#include <fstream>
#include <map>
#include <memory>

namespace ns_scanner {
    typedef std::shared_ptr<std::ifstream> sp_ifstream;
    typedef std::shared_ptr<std::stringstream> sp_sstream;

    struct Character {
        char c;
        sp_ifstream file;
        sp_sstream  string;
        bool is_file;
        int index, line, col, line_start;

        operator char() const;
        std::string to_string() const;

        static const std::string table_header;
        static const std::map<char, std::string> character_names;
    };
}


#endif //ISLANG_CHARACTER_H
