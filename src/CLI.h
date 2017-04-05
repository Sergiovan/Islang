//
// Created by Sergiovan on 18-Mar-17.
//

#ifndef ISLANG_CLI_H
#define ISLANG_CLI_H

#include "Interpreter.h"
#include "Parser.h"
#include "AST.h"

namespace ns_cli {

    class CLI {
    public:
        CLI();

        void execute(std::string input);

    private:
        ns_interpreter::Interpreter itp;
        std::string file, cwd;
        ns_ast::AST_p program;
        bool parsed = false;
    };
}

#endif //ISLANG_CLI_H
