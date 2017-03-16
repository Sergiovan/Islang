#ifdef __WIN32__
#define _WIN32_WINNT 0x0500
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x04
#endif

#endif

#include <csignal>
#include <iostream>
#include <locale>


#include "AST.h"
#include "Character.h"
#include "Color.h"
#include "Interpreter.h"
#include "Scanner.h"
#include "Lexer.h"
#include "Parser.h"
#include "InterpreterError.h"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "UTF-8");

#ifdef __WIN32__
    {
        /* Garbage needed for windows console to take ANSI codes */
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleOutputCP(65001);
        SetConsoleMode(hOut, dwMode);
    }
#endif

    signal(SIGSEGV, [](int){
        std::cout << "SIGSEGV caught, aborting process" << std::endl;
        exit(1);
    });

    if(argc < 2){
        std::cout << "Please specify the file to interpret as an argument to this program. "
                     "For example: \"Islang example.isl\" or \"Islang path/to/example.isl\"" << std::endl;
        return 1;
    }

    std::string file(argv[1]);
    auto slash = file.find_last_of('/');
    std::string cwd;
    if(slash != std::string::npos){
        cwd = file.substr(0, slash);
    }else{
        cwd = "";
    }

    {
        std::ifstream filestream(file);
        if (!filestream.good()) {
            std::cout << "File does not exist" << std::endl;
            return -1;
        }
    } //TODO BETTER

    ns_interpreter::Interpreter itp(cwd);
    ns_ast::AST *program = nullptr;
    {
        ns_parser::Parser p;
        program = p.read_file(file);
    }
    if(program){
        try {
            itp.interpret(program);
            itp.generate();
        }catch(ns_interpreter::InterpreterError& ex){
            std::cout << ex.what();
        }
        std::string input;
        std::getline(std::cin, input);

        while(input != "exit"){
            if(input == "run"){
                itp.generate();
            }
            std::getline(std::cin, input);
        }

        return 0;
    }else{
        return 1;
    }

/*    ns_lexer::Lexer lex;

    auto tokens = lex.read_file("dat/example.isl");
    if(lex.okay()) {
        std::cout << ns_lexer::Token::table_header << std::endl;
        bool last = false;
        for (auto &token : tokens) {
            if (token.get_type() == grammar::token_type::WHITESPACE && last) { ; //Nothing
            } else if (token.get_type() == grammar::token_type::WHITESPACE && !last) {
                last = true;
                std::cout << token.print() << std::endl;
            } else {
                last = false;
                std::cout << token.print() << std::endl;
            }
        }
    }else{
        std::cout << std::endl << color::ansi(color::FG_BRED) << "Error during lexing, aborting."
                  << color::ansi(color::ALL_RESET) << std::endl;
        return 1;
    }*/
}