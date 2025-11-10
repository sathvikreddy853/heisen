#ifndef HEISEN_SEMANTIC_TABLE_HPP
#define HEISEN_SYMBOL_TABLE_HPP

#include <ast.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

struct Symbol {
    Type* type; //type of the node
    Decl* declaration;    //vale of the node
};

class SymbolTable { 
    private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes; //symbol table
    public:
    SymbolTable(){
        enterscope();
    }

    void enterscope();
    void exitscope();
    bool insert(const std::string& name,const Symbol& Sym );
    const SymbolTable* find(const std::string& name) const;
};

#endif