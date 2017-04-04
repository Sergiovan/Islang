//
// Created by Sergiovan on 11-Mar-17.
//
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <stack>

#include "Compat.h"
#include "Color.h"
#include "Interpreter.h"
#include "InterpreterException.h"
#include "InterpreterError.h"
#include "Scope.h"
#include "Variable.h"
#include "Parser.h"

namespace ns_interpreter {
    Interpreter::Interpreter(std::string cwd) : cwd(cwd){
        using namespace ns_lexer;
        global = current = new Scope();
        special = new Scope();
        start = nullptr;

        special->add("red", new ns_variable::Variable(new ns_ast::AST(91, Token()), ns_ast::VALUE));
        special->add("dark_red", new ns_variable::Variable(new ns_ast::AST(31, Token()), ns_ast::VALUE));
        special->add("green", new ns_variable::Variable(new ns_ast::AST(92, Token()), ns_ast::VALUE));
        special->add("dark_green", new ns_variable::Variable(new ns_ast::AST(32, Token()), ns_ast::VALUE));
        special->add("yellow", new ns_variable::Variable(new ns_ast::AST(93, Token()), ns_ast::VALUE));
        special->add("dark_yellow", new ns_variable::Variable(new ns_ast::AST(33, Token()), ns_ast::VALUE));
        special->add("blue", new ns_variable::Variable(new ns_ast::AST(94, Token()), ns_ast::VALUE));
        special->add("dark_blue", new ns_variable::Variable(new ns_ast::AST(34, Token()), ns_ast::VALUE));
        special->add("magenta", new ns_variable::Variable(new ns_ast::AST(95, Token()), ns_ast::VALUE));
        special->add("dark_magenta", new ns_variable::Variable(new ns_ast::AST(35, Token()), ns_ast::VALUE));
        special->add("cyan", new ns_variable::Variable(new ns_ast::AST(96, Token()), ns_ast::VALUE));
        special->add("dark_cyan", new ns_variable::Variable(new ns_ast::AST(36, Token()), ns_ast::VALUE));
        special->add("gray", new ns_variable::Variable(new ns_ast::AST(97, Token()), ns_ast::VALUE));
        special->add("dark_gray", new ns_variable::Variable(new ns_ast::AST(37, Token()), ns_ast::VALUE));

        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        mt = std::mt19937(seed);
    }

    ns_ast::AST* Interpreter::interpret(ns_ast::AST* node, bool all_the_way){
        using namespace ns_ast;
        AST* ret = nullptr;
        switch(node->type){
            case NUMBER:
            case STRING:
            case BOOLEAN:
            case ENUM:
            case VALUE:
                return node;
            case LIST:
                return i_list(node);
            case CSTRING:
                ret = i_cstring(node);
                break;
            case VARIABLE:
                ret = i_variable(node);
                break;
            case BINARY:
                ret = i_binary(node);
                break;
            case UNARY:
                ret = i_unary(node);
                break;
            case BLOCK:
                ret = i_block(node);
                break;
            case NONE:
                die(node, "Encountered null node");
                break;
        }
        if(all_the_way && ret != node){
            return interpret(ret, true);
        }else{
            return ret;
        }
    }

    void Interpreter::generate(std::string what){
        Scope* s = start;
        if(what != ""){
            s = get_scope(what, nullptr);
            if(!s){
                throw InterpreterException("Scope " + what + " cannot be found");
            }
        }
        global->reset(true);
        if(!s){
            throw InterpreterException("No starting scope set (`generate` was not called)");
        }
        _generate(s, s->props.level);
    }

    std::string Interpreter::stringified(ns_ast::AST* node){
        using namespace ns_ast;
        std::stringstream ss;
        if(!node){
            throw InterpreterError(error(node, "Invalid stringify target"));
        }
        switch(node->type){
            case NUMBER:
                ss << std::fixed << std::setprecision(3) << node->val.n;
                break;
            case STRING:
                ss << node->val.s.substr(1, node->val.s.length() - 2);
                break;
            case BOOLEAN:
                ss << (node->val.b ? "true" : "false");
                break;
            case ENUM:
                ss << "{";
                for(auto it = node->val.e.begin(); it != node->val.e.end(); ++it){
                    ss << it->first;
                    if(it != --node->val.e.end()){
                        ss << ", ";
                    }
                }
                ss << "}";
                break;
            case VALUE:
                ss << node->val.v;
                break;
            case LIST:
                ss << "[";
                for(auto it = node->val.l.nodes.begin(); it != node->val.l.nodes.end(); ++it){
                    ns_ast::AST* val = *it;
                    if(val->type == BINARY && val->val.bin.op == grammar::S_IF){
                        ss << stringified(val->val.bin.left);
                        ss << " ~ ";
                        ss << stringified(val->val.bin.right->val.un.operand);
                        ss << val->val.bin.right->val.un.op;
                    }else{
                        ss << stringified(*it);
                    }
                    if(it != node->val.l.nodes.end() - 1){
                        ss << ", ";
                    }
                }
                ss << "]";
                break;
            /* These only happen from CLI. If not, something has gone terribly wrong */
            case CSTRING:
            case VARIABLE:
            case BINARY:
            case UNARY:
            case BLOCK:
                ss << stringified(interpret(node, true));
                break;
            case NONE:
                throw InterpreterError(error(node, "Invalid stringify target"));
        }
        return ss.str();
    }

    bool Interpreter::truthyness(ns_ast::AST* node){
        using namespace ns_ast;
        if(!node){
            return false;
        }
        switch(node->type){
            case NUMBER:
                return node->val.n != 0;
            case STRING:
                return node->val.s != "";
            case BOOLEAN:
                return node->val.b;
            case LIST:
                return node->val.l.nodes.size() != 0;
            case ENUM:;
            case VALUE:
            case VARIABLE:
            case CSTRING:
            case BINARY:
            case UNARY:
            case BLOCK:
                return true;
            case NONE:
                return false;
        }
        return false;
    }

    bool Interpreter::equal(ns_ast::AST* left, ns_ast::AST* right){
        using namespace ns_ast;
        AST* ileft  = interpret(left, true),
           * iright = interpret(right, true);
        if(ileft->type != iright->type){
            return false;
        }else{
            switch(ileft->type){
                case NUMBER:
                    return ileft->val.n == iright->val.n;
                case STRING:
                    return ileft->val.s == iright->val.s;
                case BOOLEAN:
                    return ileft->val.b == iright->val.b;
                case LIST:
                    if(ileft->val.l.nodes.size() != iright->val.l.nodes.size()){
                        return false;
                    }
                    for(size_t i = 0; i < ileft->val.l.nodes.size(); i++){
                        if(!equal(ileft->val.l.nodes[i], iright->val.l.nodes[i])){
                            return false;
                        }
                    }
                    return true;
                case VALUE:
                    return ileft->val.v == iright->val.v;
                case ENUM:
                case VARIABLE:
                case CSTRING:
                case BINARY:
                case UNARY:
                case BLOCK:
                case NONE:
                    return false;
            }
        }
        return false;
    }

    float Interpreter::random_number(float limit){
        return std::uniform_real_distribution<float>(0.0f, limit)(mt);
    }

    int Interpreter::random_int(int limit){
        return std::uniform_int_distribution<int>(0, limit)(mt);
    }

    std::string Interpreter::error(ns_ast::AST *node, std::string message) {
        using namespace color;
        int col = node->token.get_col();
        int line = node->token.get_line();
        std::string content = node->token.to_string();
        std::stringstream ss;
        ss << ansi(FG_BRED) << "[INTERPRETER ERROR] Line " << line << ", Col ";
        ss << col << ": " << message << std::endl;
        ss << node->token.get_full_line() << std::endl;
        ss << std::setw(col - 1) << std::string(node->offset_left, '~');
        ss << std::string(content.length(), '^') << std::string(node->offset_right, '~') << ansi(ALL_RESET);
        return ss.str();
    }

    void Interpreter::die(ns_ast::AST *node, std::string message) {
        throw InterpreterException(error(node, message));
    }

    void Interpreter::_generate(Scope* s, int startlevel){
        if(!s->props.is_block){
            return;
        }
        if(s->props.appear && s != start){
            if(!truthyness(interpret(s->props.appear, true))){
                return;
            }
        }

        int color = s->props.color;
        int indent = (s->props.level - startlevel) * 4;
        std::string& name = s->props.name;
        std::stringstream ss;
        ss << color::ansi(color) << std::string(indent, ' ') << grammar::block_type_names[s->props.type] << " - " << name;
        Scope* c = current;
        current = s;

        if(s->props.repr.size() > 0 || s->has_subscopes()){
            ss << ":" << std::endl;
            bool last = false;
            for(auto& elem : s->props.repr){
                if(elem.second && last){
                    continue;
                }
                last = false;
                std::string to_print = stringified(interpret(elem.first, true));
                if(to_print.empty()){
                    continue;
                }
                ss << std::string(indent + 4, ' ') << to_print << std::endl;
                last = true;
            }
        }else{
            ss << std::endl;
        }
        ss << color::ansi(color::ALL_RESET);
        std::cout << ss.str();
        for(const auto& elem : s->get_subscopes()){
            _generate(elem.second, startlevel);
        }
        current = c;
    }

    ns_ast::AST *Interpreter::i_cstring(ns_ast::AST *node) {
        if(!node || node->type != ns_ast::CSTRING){
            throw InterpreterError("Node is not a CSTRING");
        }
        std::stringstream ss;
        ss << '"';
        for(auto& elem : node->val.cs){
            ss  << stringified(interpret(elem, true));
        }
        ss << '"';
        return new ns_ast::AST(ss.str(), node->token, node->offset_left, node->offset_right);
    }

    ns_ast::AST *Interpreter::i_list(ns_ast::AST *node) {
        if(!node || node->type != ns_ast::LIST){
            throw InterpreterError("Node is not a LIST");
        }
        if(node->val.l.nodes.size() != 0 && node->val.l.type == ns_ast::NONE) { //Has not been properly initialized
            if(l_elem_conforms(node)) {
                return node;
            }else{
                die(node, "List is malformed");
            }
        }else{
            return node;
        }
        return nullptr;
    }

    bool Interpreter::l_elem_conforms(ns_ast::AST* node){
        if(node->val.l.nodes.size() == 0){
            return true;
        }
        int levels = 0;
        ns_ast::node_type type = ns_ast::NONE;
        std::vector<bool> prob;

        ns_ast::AST* level = node;
        while(level->type == ns_ast::LIST){
            levels++;
            if(level->val.l.nodes.size() == 0){
                break;
            }
            level = level->val.l.nodes[0];
            if(level->type == ns_ast::BINARY && level->val.bin.op == grammar::S_IF){
                prob.push_back(true);
                level = level->val.bin.left;
            }else{
                prob.push_back(false);
            }
            if(level->type != ns_ast::LIST){
                type = interpret(level, true)->type;
                break;
            }
        }
        node->val.l.type = node->val.l.nodes[0]->type;
        for(auto& elem : node->val.l.nodes){
            if(!l_elem_conforms(elem, levels - 1, type, prob)){
                return false;
            }
        }
        return true;
    }

    bool Interpreter::l_elem_conforms(ns_ast::AST* node, int level, ns_ast::node_type type, std::vector<bool>& prob){
        if(node->type == ns_ast::BINARY){
            if(node->val.bin.op == grammar::S_IF && !prob[(prob.size() - 1) - level]){
                return false;
            }else if(node->val.bin.op != grammar::S_IF && prob[(prob.size() - 1) - level]){
                return false;
            }
            if(node->val.bin.op == grammar::S_IF){
                node = node->val.bin.left;
            }
        }
        if(node->type == ns_ast::LIST){
            if(level == 0){
                return false;
            }
            node->val.l.type = level > 1 ? ns_ast::LIST : type;
            if(node->val.l.nodes.size() == 0 && level > 1){
                return false;
            }
            for(auto& elem : node->val.l.nodes){
                if(!l_elem_conforms(elem, level - 1, type, prob)){
                    return false;
                }
            }
            return true;
        }else{
            if(level != 0){
                return false;
            }
            if(type != interpret(node, true)->type){
                return false;
            }
            return true;
        }
    }

    ns_ast::AST *Interpreter::i_enumeration(ns_ast::AST *node) {
        return nullptr;
    }

    ns_ast::AST *Interpreter::i_variable(ns_ast::AST *node) {
        if(!node || node->type != ns_ast::VARIABLE){
            throw InterpreterError("Node is not a VARIABLE");
        }
        /* Commented out because of variable importing. TODO Find a workaround*/
        //if(node->val.var){
        //    return node->val.var->get_value(*this);
        //}else{
            varscope vs = get_var(node->val.s, node);
            ns_variable::Variable* var = vs.v;
            if(!var){
                die(node, "Variable has not been declared before trying to access it");
            }
         //   node->val.var = var;
            Scope* cur = current;
            current = vs.s;
            ns_ast::AST* val = var->get_value(*this);
            current = cur;
            return val;
        //}
    };

    ns_ast::AST *Interpreter::i_unary(ns_ast::AST *node) {
        if(!node || node->type != ns_ast::UNARY){
            throw InterpreterError("Node is not UNARY");
        }
        using namespace grammar;
        using namespace ns_ast;
        using namespace ns_variable;

        std::string& op = node->val.un.op;
        ns_ast::AST* operand = node->val.un.operand;

        if(op == K_IMPORT){
            if(operand->type != BINARY || operand->val.bin.op != K_AS){
                die(node, "Malformed import statement");
            }
            AST* what = operand->val.bin.left,
               * that = operand->val.bin.right;
            if((what->type != VARIABLE && what->type != STRING) ||
               (that->type != VARIABLE)){
                die(node, "Import statement not using correct identifiers");
            }
            auto begin = what->val.s.find_first_not_of("\"");
            auto end = what->val.s.find_last_not_of("\"");
            std::string import = what->val.s.substr(begin, end - begin + 1);

            auto as_begin  = that->val.s.find_last_of("/");
            auto as_end    = that->val.s.find_last_of(".");
            std::string as   = that->val.s; //TODO REDO REDO REDO
            std::string ncwd = that->val.s;
            if(as_begin != std::string::npos){
                as = as.substr(as_begin + 1);
                ncwd = ncwd.substr(0, as_begin);
            }
            if(as_end != std::string::npos){
                as = as.substr(0, as_end - as_begin - 1);
            }

            bool global = false;
            std::string gen_import = cwd + "/" + import + (what->type == STRING ? "" : ".isl");
            {
                std::ifstream in(gen_import);
                if (!in.good()) {
                    in = std::ifstream(import);
                    global = true;
                    if (!in.good()) {
                        die(node, "\"" + import + "\" cannot be found from \"" + cwd + "\"");
                    }
                }
            }
            Scope* imported = new Scope(current);
            Interpreter::import++;
            bool relative = compat::isRelativePath(ncwd);
            std::string scwd = cwd;
            cwd = relative ? cwd + ncwd : ncwd;
            current->add(as, imported);
            go_in(as);

            ns_parser::Parser p;
            AST* program_imported = p.read_file(global ? import : gen_import);
            interpret(program_imported);

            go_out();
            cwd = scwd;
            Interpreter::import--;
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_NAME){
            AST* io = interpret(operand, true);
            if(!io || io->type != STRING){
                die(node, "Name is not a string");
            }
            if(current->props.nameset){
                die(node, "Current scope already has a name");
            }
            current->props.name = stringified(io);
            current->props.nameset = true;
            return new AST(true, node->token, node->offset_left, node->offset_right);;
        }else if(op == K_APPEAR){
            AST* io = interpret(operand, true);
            if(!io || io->type != BOOLEAN){
                die(node, "Appear is not a boolean");
            }
            if(current->props.appear){
                die(node, "Current scope already has an appear value");
            }
            current->props.appear = operand;
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_COLOR){
            AST* io = interpret(operand, true);
            if(!io || io->type != VALUE){
                die(node, "Color is not a value");
            }
            if(current->props.color != 0){
                die(node, "Current scope already has a color");
            }
            current->props.color = io->val.v;
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_GENERATE){
            if(operand->type != VARIABLE){
                die(operand, "Operand not a valid identifier");
            }
            Scope* s = get_scope(operand->val.s, node);
            if(!s){
                die(operand, "Block does not exist");
            }
            if(!s->props.is_block){
                die(operand, "Block is not really a block");
            }
            if(import > 0){ //Skip if importing
                return new AST(false, node->token, node->offset_left, node->offset_right);
            }
            if(start){
                die(node, "Already issued generate");
            }
            start = s;
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_REPR){
            AST* io = interpret(operand, true);
            if(!io){
                die(node, "REPR result is not a string");
            }
            current->props.repr.emplace_back(operand, false);
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_ELSE){
            if(operand->type != UNARY || (operand->val.un.op != K_REPR && operand->val.un.op != K_OPTS)){
                die(node, "Else followed by invalid argument");
            }
            interpret(operand);
            current->props.repr.pop_back();
            current->props.repr.emplace_back(operand->val.un.operand, true);
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_OPTS){
            AST* io = interpret(operand, true);
            if(!io){
                die(node, "Options are not strings");
            }
            current->props.repr.emplace_back(operand, false);
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_USE){
            if(operand->type != BINARY || operand->val.bin.op != K_AS){
                die(node, "Malformed use statement");
            }
            AST* what = operand->val.bin.left,
               * that = operand->val.bin.right;
            if(what->type != VARIABLE || (that->type != VARIABLE)){
                die(node, "Use statement not using correct identifiers");
            }
            varscope vs = get_var_scope(what->val.s, what, true);
            if(!vs.s && !vs.v){
                die(node, "No such scope/variable exists");
            }
            if(vs.v){ //Tis but a variable
                vs = get_var(what->val.s, what); //Redundant, but needed
                current->import(that->val.s, vs.v, vs.s);
            }else{
                current->import(that->val.s, vs.s);
            }
            return new AST(true, node->token, node->offset_left, node->offset_right);
        }else if(op == K_NOT){
            return new AST(!truthyness(interpret(operand, true)), operand->token);
        }else if(op == S_PERCENTAGE){
            if(operand->type != NUMBER){
                die(node, "Wrong value for percentages");
            }
            if(operand->val.n > 100.f || operand->val.n < 0.f){
                die(operand, "Invalid percentage");
            }
            return new AST(random_number() < operand->val.n, node->token, node->offset_left, node->offset_right);
        }else if(op == S_CARET){
            if(operand->type != NUMBER){
                die(node, "Wrong value for percentages");
            }
            if(operand->val.n < 0.f){
                die(operand, "Invalid number");
            }
            return new AST(random_number() < ((1.f / operand->val.n) * 100.f), node->token, node->offset_left, node->offset_right);
        }else if(op == S_SPECIAL_SCOPE){
            if(operand->type != VARIABLE){
                die(node, "What the fuck man, I trusted you");
            }
            Scope* c = current;
            current = special;
            AST* ret = interpret(operand, true);
            current = c;
            return ret;
        }else{
            die(node, "Invalid operand " + op);
        }
        return nullptr;
    }

    ns_ast::AST *Interpreter::i_binary(ns_ast::AST *node) {
        if(!node || node->type != ns_ast::BINARY){
            throw InterpreterError("Node is not BINARY");
        }
        using namespace grammar;
        using namespace ns_ast;
        using namespace ns_variable;

        std::string& op = node->val.bin.op;
        AST* left = node->val.bin.left;
        AST* right = node->val.bin.right;

        if(op == S_IS){
            if(left->type != UNARY && left->type != VARIABLE){
                die(node, "Invalid declaration or assignment");
            }

            if(left->type == UNARY && left->val.un.op == S_SPECIAL_SCOPE){
                die(node, "Cannot assign to variable in special scope");
            }

            Variable* var = nullptr;
            AST* rresult = interpret(right, true);

            if(left->type == UNARY){
                node_type new_type = NONE;

                if(current->find_var(left->val.un.operand->val.s)){
                    die(left, "Variable already exists");
                }

                std::string& type_name = left->val.un.op;
                if(type_name == K_BOOL){
                    new_type = BOOLEAN;
                }else if(type_name == K_NUM){
                    new_type = NUMBER;
                }else if(type_name == K_STR){
                    new_type = STRING;
                }else if(type_name == K_ENUM){
                    new_type = ENUM;
                }else if(type_name == K_VAL){
                    new_type = VALUE;
                }else if(type_name == K_LIST){
                    new_type = LIST;
                }else{
                    die(left, "Undefined typename");
                }
                if(new_type != rresult->type){
                    die(node, "Variable type and assigned type mismatch");
                }
                var = new Variable(right, new_type);
                current->add(left->val.un.operand->val.s, var);

                if(new_type == ENUM){
                    Scope* escope = new Scope(current);
                    current->add(left->val.un.operand->val.s, escope);

                    int cval = 0;
                    for(auto& e : rresult->val.e){
                        escope->add(e.first, new Variable(new AST(cval++, right->token, right->offset_left, right->offset_right), VALUE));
                    }
                }

            }else{
                var = get_var(left->val.s, left).v;//current->find_var(left->val.s, false);
                if(!var){
                    die(left, "Variable does not exist");
                }else if(var->get_type() == ENUM){
                    die(node, "Trying to assign to enum");
                }else if(var->get_type() != rresult->type){
                    die(node, "Variable type and assigned type mismatch");
                }
                var->reset();
                var->set_value(right);
            }
            return new AST(var, left->token, left->offset_left, left->offset_right);
        }else if(op == S_SPECIAL_SCOPE){
            if(left->type != VARIABLE){
                die(left, "Argument is not an identifier");
            }else if(right->type != LIST){
                die(right, "Argument is not a list-like");
            }
            varscope vs = get_var(left->val.s, left);
            if(!vs.v || vs.v->get_type() != ENUM){
                std::cout << vs.v->get_type() << std::endl;
                die(left, "Argument is not an enum"); //TODO List compatible?
            }
            AST* e = vs.v->get_raw_value();
            if(e->val.e.size() != right->val.l.nodes.size()){
                die(node, "Probability list and enum don't have same amount of elements");
            }
            std::vector<AST*> nodes;
            for(size_t i = 0; i < right->val.l.nodes.size(); i++){
                nodes.push_back(new AST(S_IF, new AST((int) i, right->val.l.nodes[i]->token), right->val.l.nodes[i], right->val.l.nodes[i]->token));
            }

            return new AST(std::move(nodes), VALUE, node->token, node->offset_left, node->offset_right); // TODO Make better
        }else if(op == S_PICK || op == S_EXPICK || op == S_UNBIASED_PICK || op == S_UNBIASED_EXPICK) {
            AST *ileft = interpret(left, true),
                    *iright = interpret(right, true),
                    *amount = nullptr, *list = nullptr;
            if (ileft->type == NUMBER && iright->type == LIST) {
                amount = ileft;
                list = iright;
            } else if (ileft->type == LIST && iright->type == NUMBER) {
                amount = iright;
                list = ileft;
            } else {
                die(left, "Illegal argument types");
            }

            float num = amount->val.n;
            if (num != std::floor(num)) {
                die(amount, "Argument is not an integer");
            }
            if (num < 1) {
                die(amount, "Invalid amount");
            }

            if(list->val.l.nodes.size() == 0){
                die(list, "List is empty");
            }

            enum OType {
                PICK = 0, EXPICK, UPICK, UEXPICK
            } otype;

            struct ce {
                int element;
                float chance;
            };

            otype = op[0] == '*' ? PICK : EXPICK;
            otype = num == 1 ? PICK : otype;
            otype = (OType) (otype + (op.size() == 2 ? 2 : 0));
            otype = (OType) (list->val.l.nodes[0]->type != BINARY || list->val.l.nodes[0]->val.bin.op != S_IF ?
                             (otype % 2) + 2 : otype);

            auto list_size = list->val.l.nodes.size();
            float total_chance = 0.0f;

            ce* nodes = new ce[list_size];
            for(size_t i = 0; i < list_size; i++){
                auto& elem = list->val.l.nodes[i];
                float chance = 0.0f;
                if(elem->type == BINARY && elem->val.bin.op == S_IF && otype < 2){
                    std::string& sop = elem->val.bin.right->val.un.op;
                    if(sop == S_PERCENTAGE){
                        chance = elem->val.bin.right->val.un.operand->val.n;
                    }else{
                        chance = (1.f / elem->val.bin.right->val.un.operand->val.n) * 100.f;
                    }
                    total_chance += chance;
                }
                nodes[i] = {(int) i,  chance};
            };

            int rngl = list_size;
            std::vector<int> selected;
            for(int i = 0; i < num; i++){
                if(otype < 2){ //Biased
                    float chosen = -1;
                    chosen = random_number(total_chance);
                    float accumulated = 0.0f;
                    for(int j = 0; j < rngl; j++){
                        if(nodes[j].chance + accumulated > chosen){
                            selected.push_back(nodes[j].element);
                            if(otype == EXPICK || otype == UEXPICK){
                                total_chance -= nodes[j].chance;
                                nodes[j] = nodes[--rngl];
                            }
                        }else{
                            accumulated += nodes[j].chance;
                        }
                    }
                }else{ //Unbiased
                    int chosen;
                    chosen = random_int(rngl - 1);
                    selected.push_back(nodes[chosen].element);
                    if(otype == EXPICK || otype == UEXPICK){
                        nodes[(int) chosen] = nodes[--rngl];
                    }
                }
            }
            delete [] nodes;
            nodes = nullptr;
            if(num == 1){
                AST* ret = list->val.l.nodes[selected[0]];
                if(ret->type == BINARY && ret->val.bin.op == S_IF){
                    ret = ret->val.bin.left;
                }
                return new AST(*ret);
            }else{
                std::vector<AST*> ret;
                for(auto& i : selected){
                    AST* elem = list->val.l.nodes[i];
                    if(elem->type == BINARY && elem->val.bin.op == S_IF){
                        elem = elem->val.bin.left;
                    }
                    ret.push_back(elem);
                }
                return new AST(std::move(ret), list->val.l.type, node->token, node->offset_left, node->offset_right);
            }

        }else if(op == S_IF){
            AST* intd = interpret(left, true);
            if(truthyness(interpret(right, true))){
                return intd;
            }else{
                return new AST(std::string("\"\""), node->token, node->offset_left, node->offset_right);
            }
        }else if(op == S_EQUALS || op == S_NOT_EQUALS){
            bool are_equal = equal(left, right);
            return new AST(are_equal == (op == S_EQUALS), node->token, node->offset_left, node->offset_right);
        }else if(op == K_AND || op == K_OR ){
            bool is_and = op == K_AND;
            bool left_val = truthyness(interpret(left, true));
            if(left_val != is_and){
                return new AST(left_val, node->token, node->offset_left, node->offset_right);
            }
            return new AST(truthyness(interpret(right, true)), right->token);
        }else if(op == S_SQBRACKETS){
            AST* ileft = interpret(left, true);
            if((ileft->type != LIST) || right->type != NUMBER){
                die(node, "Illegal indexing");
            }

            if(right->val.n < 0){
                die(node, "Indexing negative element");
            }else if(right->val.n != std::floor(right->val.n)){
                die(node, "Indexing with a float value");
            }

            if(ileft->type != LIST){
                die(node, "Indexing non-list");
            }else if(ileft->val.l.nodes.size() < right->val.n){
                die(node, "Index out of range");
            }

            return ileft->val.l.nodes[(int) right->val.n];
        }else{
            die(node, "Unknown operator");
        }

        return nullptr;
    }

    ns_ast::AST *Interpreter::i_block(ns_ast::AST *node) {
        if(!node || node->type != ns_ast::BLOCK){
            throw InterpreterError("Node is not a BLOCK");
        }
        if(node->val.blk.type != ns_ast::PROGRAM){
            Scope* ns = new Scope(current);
            properties& props = ns->props;
            props.is_block = true;
            props.name = node->val.blk.name;
            props.type = node->val.blk.type;
            props.level = current->props.level + 1;

            current->add(node->val.blk.name, ns);
            go_in(node->val.blk.name);
        }

        for(auto& stmt : node->val.blk.statements){
            try {
                interpret(stmt);
            }catch(InterpreterException& ex){
                std::cout << ex.what() << std::endl;
            }
        }

        if(node->val.blk.type != ns_ast::PROGRAM){
            go_out();
        }

        return node;
    }

    varscope Interpreter::get_var_scope(std::string name, ns_ast::AST* node, bool var_and_scope){
        if(name.find(grammar::C_DOT) != std::string::npos){
            std::istringstream ss(name);
            std::string pre_token, token;
            std::getline(ss, pre_token, grammar::C_DOT);
            std::getline(ss, token, grammar::C_DOT);
            Scope* cur = current;
            Scope* start = cur->find_scope(pre_token);
            if(!start && cur == global){
                die(node, "Scope \"" + pre_token + "\" cannot be found");
            }
            while(!start){
                cur = cur->get_parent();
                start = cur->find_scope(pre_token);
                if(!start && cur == global){
                    die(node, "Scope \"" + pre_token + "\" cannot be found");
                }
            }
            pre_token = token;
            while(std::getline(ss, token, grammar::C_DOT)){
                start = start->find_scope(pre_token);
                if(!start){
                    die(node, "Scope \"" + pre_token + "\" cannot be found");
                }
                pre_token = token;
            }
            varscope vs = start->find_varscope(pre_token, false);
            if(var_and_scope) {
                vs.s = start->find_scope(pre_token, false);
            }
            return vs;
        }else{
            varscope vs = current->find_varscope(name, true);
            if(var_and_scope) {
                vs.s = current->find_scope(name);
            }
            return vs;
        }
    }

    varscope Interpreter::get_var(std::string name, ns_ast::AST* node){
        return get_var_scope(name, node, false);
    }

    Scope* Interpreter::get_scope(std::string name, ns_ast::AST* node){
        return get_var_scope(name, node, true).s;
    }

    void Interpreter::go_in(std::string name){
        Scope* into = current->find_scope(name);
        if(!into){
            throw InterpreterError("Scope does not exist: " + name + ". Current location: " + scope_name());
        }
        current_name.push_back(name);
        current = into;
    }

    void Interpreter::go_out(){
        current = current->get_parent();
        if(!current){
            throw InterpreterError("Left global scope (INTO SPACE!)");
        }
        current_name.pop_back();
    }

    std::string Interpreter::scope_name(){
        std::stringstream ss;
        ss << "$.";
        std::copy(current_name.begin(), current_name.end(), std::ostream_iterator<std::string>(ss, "."));
        return ss.str();
    }
}