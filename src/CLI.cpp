//
// Created by Sergiovan on 18-Mar-17.
//

#include <algorithm>
#include <fstream>
#include <iostream>
#include "CLI.h"


namespace ns_cli{
    CLI::CLI() : itp(""), file(""), cwd(""), program(nullptr){

    }

    void CLI::execute(std::string input) {
        using std::cout;
        using std::endl;
        using std::string;

        auto first_non_space = input.find_first_not_of(" \t\n\r");
        input = first_non_space != string::npos ? input.substr(first_non_space) : input;
        auto first_space = input.find_first_of(' ', input.find_first_not_of(' '));
        first_space = first_space != string::npos ? first_space : input.length();
        string command = input.substr(0, first_space);
        string rest = first_space != input.length() ? input.substr(first_space + 1) : "";
        if(command == "load" || command == "l"){
            file = rest;
            std::replace(file.begin(), file.end(), '\\', '/');
            auto slash = file.find_last_of("/");
            cwd = slash != string::npos ? file.substr(0, slash) : "";
            std::ifstream filestream(file);
            if (!filestream.good()) {
                cout << "File does not exist" << endl;
                return;
            }
            ns_parser::Parser prs;
            program = prs.read_file(file);
            parsed = false;
            cout << "Successfully loaded \"" << file << "\"" << endl;
        }else if(command == "parse" || command == "p"){
            if(!program){
                cout << "No program loaded" << endl;
                return;
            }
            itp = ns_interpreter::Interpreter(cwd);
            try {
                itp.interpret(program);
            }catch(std::exception const& ex){
                cout << ex.what() << endl;
                cout << "Parsing unsuccessful" << endl;
                return;
            }
            parsed = true;
        }else if(command == "lp"){
            execute("l " + rest);
            if(program){
                execute("p");
            }
        }else if(command == "run" || command == "r"){
            if(!parsed && rest.empty()){
                cout << "Program not parsed correctly" << endl;
                return;
            }
            try {
                itp.generate(rest);
            }catch(std::exception const& ex){
                cout << ex.what() << endl;
                cout << "Could not generate" << endl;
                return;
            }
        }else if(command == "execute" || command == "e"){
            ns_ast::AST* line = nullptr;
            ns_parser::Parser prs;
            line = prs.read_string(rest);
            if(!line){
                cout << "Could not execute" << endl;
                return;
            }
            try {
                cout << itp.stringified(itp.interpret(line, true)) << endl;
            }catch(std::exception const& ex) {
                cout << ex.what() << endl;
                cout << "Parsing unsuccessful" << endl;
            }
        }else{
            cout << "Unrecognized command \"" << command << "\"" << endl;
        }
        //Probably set or return an error here?
    }

}