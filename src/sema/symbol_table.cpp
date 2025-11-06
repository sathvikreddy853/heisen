#include <symbol_table.hpp>
#include <iostream>
#include <unordered_map>
#include <optional>

void SymbolTable::enterscope () {
    scopes.emplace_back (); // adding a new scope
}
void SymbolTable::exitscope () { 
    scopes.pop_back ();  //removing the exited scope data 
}
void SymbolTable::insert (const std::string& name, const Symbol& Sym) {
    if (scopes.size()) {  
        scopes.back ()[name] = Sym;  //inserting the new data member
    }
}
std::optional<Symbol> SymbolTable::find(const std::string& name)const{
    for (int i=scopes.size()-1;i>=0;i--){
      auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            return it->second;  // return the found Symbol
        }
    }
    return {};
}
