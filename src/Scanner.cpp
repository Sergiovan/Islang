//
// Created by Sergiovan on 04-Mar-17.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "Scanner.h"

using namespace ns_scanner;

Scanner::Scanner() {
    is_file = finished = false;
    index = line_start = 0;
    line = col = 1;
}

void Scanner::open_file(std::string filename) {
    file = std::make_shared<std::ifstream>(std::ifstream(filename, std::ifstream::in | std::ifstream::binary));
    is_file = true;
    finished = false;
    index = line_start = 0;
    line = col = 1;
}

void Scanner::open_string(std::string string) {
    Scanner::string = std::make_shared<std::stringstream>(std::stringstream(string));
    is_file = finished = false;
    index = line_start = 0;
    line = col = 1;
}

Character Scanner::get() {
    if(finished){
        return {
            EOF,
            file,
            string,
            is_file,
            index, line, col, line_start
        };
    }
    char c;
    if(is_file){
        file->get(c);
        if(file->eof()){
            finished = true;
            c = EOF;
        }
    }else{
        string->get(c);
        if(string->eof()){
            finished = true;
            c = EOF;
        }
    }
    index++;
    col++;
    if(c == '\n'){
        line++;
        col = 0;
        line_start = index;
    }
    return {
        c,
        file,
        string,
        is_file,
        index, line, col, line_start
    };
}

void Scanner::discard_while(std::string chars) {
    char c = peek();
    while(chars.find(c) != std::string::npos){
        get();
        c = peek();
    }
}

char Scanner::peek() {
    return is_file ? (char) file->peek() : (char) string->peek();
}



bool Scanner::done(){
    return finished;
}