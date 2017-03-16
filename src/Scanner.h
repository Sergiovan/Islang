//
// Created by Sergiovan on 04-Mar-17.
//

#ifndef ISLANG_SCANNER_H
#define ISLANG_SCANNER_H

#include <iostream>
#include <memory>
#include "Character.h"

namespace ns_scanner {
    class Scanner {
    public:
        Scanner();
        void open_file(std::string filename);
        void open_string(std::string string);
        Character get();
        void discard_while(std::string chars);
        char peek();
        bool done();
    private:
        sp_ifstream file;
        sp_sstream  string;
        bool is_file, finished;
        int index, line, col, line_start;
    };
}


#endif //ISLANG_SCANNER_H
