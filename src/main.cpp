#include <csignal>
#include <cstring>
#include <iostream>
#include <locale>

#include "AST.h"
#include "Character.h"
#include "CLI.h"
#include "Color.h"
#include "Compat.h"
#include "Interpreter.h"
#include "Scanner.h"
#include "Lexer.h"
#include "Parser.h"
#include "InterpreterError.h"

std::vector<std::string> split_commands(std::string input);

int main(int argc, char** argv) {
    setlocale(LC_ALL, "UTF-8");
    compat::init();

    signal(SIGSEGV, [](int){
        std::cout << "SIGSEGV caught, aborting process" << std::endl;
        exit(1);
    });

    signal(SIGTERM, [](int){
        std::cout << "Ctrl-C detected" << std::endl;
        exit(0);
    });

    //TODO Parse command line arguments
    std::string input("");
    ns_cli::CLI cli;

    if(argc > 1){
        for(int i = 1; i < argc; i++){
            if(!std::strcmp(argv[i], "--file")){
                input = std::string("lp ") + argv[++i] + input;
            }else if(!std::strcmp(argv[i], "--run")){
                input += "r";
            }else{
                std::cout << "Unrecognized command \"" << argv[i] << "\"" << std::endl;
                continue;
            }
            input += ";";
        }
    }

    while(input != "exit"){
        if(!input.empty()) {
            for (auto &command : split_commands(input)) {
                if (!command.empty()) {
                    cli.execute(command);
                }
            }
        }
        std::cout << "> ";
        std::getline(std::cin, input);
    }


    /*

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

*    ns_lexer::Lexer lex;

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

std::vector<std::string> split_commands(std::string input){
    if(input.find(";") == std::string::npos){
        return {input};
    }else{
        std::vector<std::string> ret = {};
        std::string current = "";
        int depth = 0;
        bool escape = false, quote_close = false;
        for(auto& c : input){
            current += c;
            if(depth % 2 == 1 && c == '\\'){
                escape = true;
                continue;
            }
            if(c == '"' && !escape){
                if(quote_close){
                    depth--;
                    quote_close = false;
                }else{
                    depth++;
                    quote_close = true;
                }
                continue;
            }
            if(depth % 2 == 1 && c == '{' && !escape){
                depth++;
                quote_close = false;
                continue;
            }
            if(depth > 0 && depth % 2 == 0 && c == '}' && !escape){
                depth--;
                quote_close = true;
                continue;
            }
            escape = false;
            if(depth == 0 && c == ';'){
                current.pop_back();
                ret.push_back(std::move(current));
                current = "";
            }
        }
        ret.push_back(std::move(current));
        return ret;
    }
}