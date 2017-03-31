//
// Created by Sergiovan on 11-Mar-17.
//

#ifndef ISLANG_SCOPE_H
#define ISLANG_SCOPE_H

#include <map>
#include <utility>

#include "AST.h"
#include "Variable.h"

namespace ns_interpreter {
    class Scope;

    struct varscope {
        Scope* s;
        ns_variable::Variable* v;
    };

    struct properties {
        bool is_block, nameset;
        ns_ast::block_type type;

        ns_ast::AST* appear;
        std::string name;
        int color, level;

        std::vector<std::pair<ns_ast::AST*, bool>> repr;
    };

    class Scope {
    public:
        Scope(Scope* parent = nullptr);

        void reset(bool propagate = false);

        Scope* get_parent();
        bool has_subscopes();
        const std::map<std::string, Scope*>& get_subscopes() const;

        void add(std::string name, ns_variable::Variable* var);
        void add(std::string name, Scope *scope);
        void import(std::string name, ns_variable::Variable* var, Scope* scope);
        void import(std::string name, Scope *scope);

        ns_variable::Variable* find_var(std::string name, bool propagate = false);
        Scope* find_scope(std::string name, bool propagate = false);
        varscope find_varscope(std::string name, bool propagate = false);

        properties props;
    private:
        Scope* parent;
        std::map<std::string, ns_variable::Variable*> vars;
        std::map<std::string, Scope*> subscopes;
        std::map<std::string, std::pair<ns_variable::Variable*, Scope*>> imported_vars;
        std::map<std::string, Scope*> imported_scopes;
    };
}

#endif //ISLANG_SCOPE_H
