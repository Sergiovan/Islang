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
        val.var = var;
        val.s = varname;
    }

    AST::AST(int v, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        type = VALUE;
        val.v = v;
    }

    AST::AST(std::vector<AST*>&& vec, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = CSTRING;
        val.cs = vec;
    }

    AST::AST(std::vector<AST*>&& vec, node_type type, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = LIST;
        val.l = {type, vec};
    }

    AST::AST(std::vector<std::string>&& e, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = ENUM;
        val.e = e;
    }

    AST::AST(std::string op, AST* operand, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = UNARY;
        val.un = {op, operand};
    };

    AST::AST(std::string op, AST *left, AST *right, ns_lexer::Token token, int oleft, int oright) : AST(token, oleft, oright){
        AST::type = BINARY;
        val.bin = {op, left, right};
    };

    AST::AST(block_type type, std::string name, std::vector<AST *> &&statements, ns_lexer::Token token, int oleft,
             int oright)
        : AST(token, oleft, oright){
        AST::type = BLOCK;
        val.blk = {type, name, statements};
    };

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
                if(val.var){
                    //TODO
                }else{
                    indent(depth + 1);
                    cout << "Value: NULL" <<endl;
                }
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
                    cout << elem << endl;
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

    void indent(int depth){
        for(int i = 0; i < depth - 1; i++){
            std::cout << "║ ";
        }
        if(depth > 0){
            std::cout << "╠═";
        }
    }

}