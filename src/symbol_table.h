#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <map>
#include <vector>

struct Symbol {
    std::string name;
    int address;
    bool defined;
    
    Symbol() : address(-1), defined(false) {}
    Symbol(const std::string& n, int addr, bool def) 
        : name(n), address(addr), defined(def) {}
};

struct PendingReference {
    int instruction_address;  // Address where the reference occurs
    std::string symbol_name;  // Name of the referenced symbol
    int line_number;          // Line number for error reporting
    
    PendingReference(int addr, const std::string& sym, int line)
        : instruction_address(addr), symbol_name(sym), line_number(line) {}
};

class SymbolTable {
private:
    std::map<std::string, Symbol> symbols;
    std::vector<PendingReference> pending_references;
    
public:
    SymbolTable();
    
    // Symbol management
    bool addSymbol(const std::string& name, int address);
    bool symbolExists(const std::string& name) const;
    bool isSymbolDefined(const std::string& name) const;
    int getSymbolAddress(const std::string& name) const;
    void defineSymbol(const std::string& name, int address);
    
    // Pending references
    void addPendingReference(int address, const std::string& symbol, int line);
    const std::vector<PendingReference>& getPendingReferences() const;
    
    // Resolution
    std::vector<std::pair<int, int>> resolvePendingReferences();
    
    // Debugging
    void printSymbolTable() const;
    std::vector<std::string> getUndefinedSymbols() const;
};

#endif // SYMBOL_TABLE_H
