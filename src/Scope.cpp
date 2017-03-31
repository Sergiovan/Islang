//
// Created by Sergiovan on 11-Mar-17.
//

#include <utility>

#include "Interpreter.h"
#include "Scope.h"
#include "InterpreterException.h"

namespace ns_interpreter {

    Scope::Scope(Scope* parent) : parent(parent) {
        props = {
            false,
            false,
            ns_ast::NONE_BLOCK,
            nullptr,
            "",
            0,
            parent ? parent->props.level + 1 : 0,
            {}
        };
    };

    void Scope::reset(bool propagate) {
        for(auto& var : vars){
            var.second->reset();
        }
        if(propagate){
            for(auto& scope : subscopes){
                scope.second->reset(true);
            }
        }
    }

    Scope* Scope::get_parent(){
        return parent;
    }

    bool Scope::has_subscopes(){
        return subscopes.size() > 0;
    }

    const std::map<std::string, Scope*>& Scope::get_subscopes() const{
        return subscopes;
    }

    void Scope::add(std::string name, ns_variable::Variable *var) {
        if(vars.count(name) > 0){
            Interpreter::die(vars[name]->get_raw_value(), "Variable already exists");
        }
        vars[name] = var;
    }

    void Scope::add(std::string name, Scope *scope) {
        if(subscopes.count(name) > 0){
            throw InterpreterException("Scope already exists");
        }
        subscopes[name] = scope;
    }

    void Scope::import(std::string name, ns_variable::Variable *var, Scope* scope) {
        imported_vars[name] = std::pair<ns_variable::Variable*, Scope*>(var, scope);
    }

    void Scope::import(std::string name, Scope *scope) {
        imported_scopes[name] = scope;
    }

    ns_variable::Variable *Scope::find_var(std::string name, bool propagate) {
        return find_varscope(name, propagate).v;
    }

    Scope *Scope::find_scope(std::string name, bool propagate) {
        auto it = subscopes.find(name);
        if(it != subscopes.end()){
            return it->second;
        }
        it = imported_scopes.find(name);
        if(it != imported_scopes.end()){
            return it->second;
        }
        if(propagate && parent){
            return parent->find_scope(name, true);
        }else{
            return nullptr;
        }
    }

    varscope Scope::find_varscope(std::string name, bool propagate) {
        varscope vs;
        vs.s = this;
        auto it = vars.find(name);
        if(it != vars.end()){
            vs.v = it->second;
            return vs;
        }
        auto itt = imported_vars.find(name);
        if(itt != imported_vars.end()){
            vs.v = itt->second.first;
            vs.s = itt->second.second;
            return vs;
        }
        if(propagate && parent) {
            return parent->find_varscope(name, true);
        }else {
            vs.s = nullptr;
            vs.v = nullptr;
            return vs;
        }
    }
}