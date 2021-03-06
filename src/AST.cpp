//
// Created by Sergiovan on 05-Mar-17.
//

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "AST.h"
#include "Variable.h"


namespace ns_ast {

    AST::AST(ns_lexer::Token token, int oleft, int oright)
            : type(NONE), token(token), offset_left(oleft), offset_right(oright){

    }

    AST::AST(float n, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        type = NUMBER;
        val.n = n;
    }

    AST::AST(bool b, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        type = BOOLEAN;
        val.b = b;
    }

    AST::AST(std::string s, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        type = STRING;
        val.s = s;
    }

    AST::AST(ns_variable::Variable *var, std::string varname, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        type = VARIABLE;
        //val.var = var;
        val.s = varname;
    }

    AST::AST(int v, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        type = VALUE;
        val.v = v;
    }

    AST::AST(std::vector<AST_p>&& vec, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = CSTRING;
        val.cs = vec;
    }

    AST::AST(std::vector<AST_p>&& vec, node_type type, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = LIST;
        val.l = {type, vec};
    }

    AST::AST(std::vector<std::string>&& e, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = ENUM;
        for(unsigned int i = 0; i < e.size(); i++) {
            val.e[e[i]] = i;
        }
    }

    AST::AST(std::string op, AST_p operand, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = UNARY;
        val.un = {op, operand};
    }

    AST::AST(std::string op, AST_p left, AST_p right, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = BINARY;
        val.bin = {op, left, right};
    }

    AST::AST(block_type type, std::string name, std::vector<AST_p>&& statements, ns_lexer::Token token, int oleft,
             int oright)
        : AST(token, oleft, oright){
        AST::type = BLOCK;
        val.blk = {type, name, statements};
    }

    void AST::print(int depth){
        using std::cout;
        using std::endl;
        using std::setw;
        indent(depth);
        switch(type){
            case NUMBER:
                cout << "Number: " << val.n << endl;
                break;
            case BOOLEAN:
                cout << "Boolean: " << (val.b ? "True" : "False") << endl;
                break;
            case STRING:
                cout << "String: " << val.s << endl;
                break;
            case VARIABLE:
                cout << "Variable: " << val.s << endl;
                //if(val.var){
                    //TODO
                //}else{
                    indent(depth + 1);
                    cout << "Value: NULL" <<endl;
                //}
                break;
            case VALUE:
                cout << "Value: " << val.v << endl;
                break;
            case CSTRING:
                cout << "Complex string: " << endl;
                for(auto elem : val.cs){
                    elem->print(depth + 1);
                }
                break;
            case LIST:
                cout << "List: " << endl; //TODO List type
                for(auto elem : val.l.nodes){
                    elem->print(depth + 1);
                }
                break;
            case ENUM:
                cout << "Enumeration: " << endl;
                for(auto& elem : val.e){
                    indent(depth + 1);
                    cout << elem.first << endl;
                }
                break;
            case BINARY:
                cout << "Binary: (" << val.bin.op << ")" << endl;
                if(val.bin.left){
                    val.bin.left->print(depth + 1);
                }else{
                    indent(depth + 1);
                    cout << "NULL" << endl;
                }
                if(val.bin.right){
                    val.bin.right->print(depth + 1);
                }else{
                    indent(depth + 1);
                    cout << "NULL" << endl;
                }
                break;
            case UNARY:
                cout << "Unary: (" << val.un.op << ")" << endl;
                if(val.un.operand){
                    val.un.operand->print(depth + 1);
                }else{
                    indent(depth + 1);
                    cout << "NULL" << endl;
                }
                break;
            case BLOCK:
                cout << "Block: (" << val.blk.name << ") [" << grammar::block_type_names[val.blk.type] << "]" << endl;
                for(auto elem : val.blk.statements){
                    elem->print(depth + 1);
                }
                break;
            case NONE:
                cout << "NONE" << endl;
                break;
        }
    }

    bool AST::is_num() const noexcept {
        return is<NUMBER>();
    }

    bool AST::is_bool() const noexcept {
        return is<BOOLEAN>();
    }

    bool AST::is_str() const noexcept {
        return is<STRING>();
    }

    bool AST::is_cstr() const noexcept {
        return is<CSTRING>();
    }

    bool AST::is_any_str() const noexcept {
        return is<STRING, CSTRING>();
    }

    bool AST::is_val() const noexcept {
        return is<VALUE>();
    }

    bool AST::is_enum() const noexcept {
        return is<ENUM>();
    }

    bool AST::is_list() const noexcept {
        return is<LIST>();
    }

    bool AST::is_un() const noexcept {
        return is<UNARY>();
    }

    bool AST::is_bin() const noexcept {
        return is<BINARY>();
    }

    bool AST::is_blk() const noexcept {
        return is<BLOCK>();
    }


    void indent(int depth){
        for(int i = 0; i < depth - 1; i++){
            std::cout << "║ ";
        }
        if(depth > 0){
            std::cout << "╠═";
        }
    }

   /* AST::~AST() {
        std::cout << "Called ~AST()" << std::endl;
        print();
    }*/
}