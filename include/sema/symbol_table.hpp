#ifndef HEISEN_SEMANTIC_TABLE_HPP
#define HEISEN_SYMBOL_TABLE_HPP


#include <string>
#include <unordered_map>
#include <vector>
#include <optional>


struct Symbol {
    std::string type; //type of the node
    int value;    //vale of the node
};

class SymbolTable {
    private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes; //symbol table
    public:
    void enterscope();
    void exitscope();
    void insert(const std::string& name,const Symbol& Sym );
   std::optional<Symbol> find(const std::string& name) const;
};


#endif