#include "symbol_table.h"
#include <iostream>

SymbolTable::SymbolTable() {
}

bool SymbolTable::addSymbol(const std::string& name, int address) {
    if (symbols.find(name) != symbols.end() && symbols[name].defined) {
        // Symbol already defined
        return false;
    }
    
    symbols[name] = Symbol(name, address, true);
    return true;
}

bool SymbolTable::symbolExists(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

bool SymbolTable::isSymbolDefined(const std::string& name) const {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.defined;
    }
    return false;
}

int SymbolTable::getSymbolAddress(const std::string& name) const {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.address;
    }
    return -1;
}

void SymbolTable::defineSymbol(const std::string& name, int address) {
    symbols[name] = Symbol(name, address, true);
}

void SymbolTable::addPendingReference(int address, const std::string& symbol, int line) {
    pending_references.push_back(PendingReference(address, symbol, line));
    
    // Add symbol to table if it doesn't exist (as undefined)
    if (symbols.find(symbol) == symbols.end()) {
        symbols[symbol] = Symbol(symbol, -1, false);
    }
}

const std::vector<PendingReference>& SymbolTable::getPendingReferences() const {
    return pending_references;
}

std::vector<std::pair<int, int>> SymbolTable::resolvePendingReferences() {
    std::vector<std::pair<int, int>> resolutions;
    
    for (const auto& ref : pending_references) {
        if (isSymbolDefined(ref.symbol_name)) {
            int symbol_address = getSymbolAddress(ref.symbol_name);
            resolutions.push_back({ref.instruction_address, symbol_address});
        }
    }
    
    return resolutions;
}

void SymbolTable::printSymbolTable() const {
    std::cout << "\nSymbol Table:\n";
    std::cout << "Name\t\tAddress\t\tDefined\n";
    std::cout << "----\t\t-------\t\t-------\n";
    
    for (const auto& pair : symbols) {
        const Symbol& sym = pair.second;
        std::cout << sym.name << "\t\t" << sym.address << "\t\t" 
                  << (sym.defined ? "Yes" : "No") << "\n";
    }
    
    if (!pending_references.empty()) {
        std::cout << "\nPending References:\n";
        for (const auto& ref : pending_references) {
            std::cout << "Address " << ref.instruction_address 
                      << " references " << ref.symbol_name 
                      << " (line " << ref.line_number << ")\n";
        }
    }
}

std::vector<std::string> SymbolTable::getUndefinedSymbols() const {
    std::vector<std::string> undefined;
    
    for (const auto& pair : symbols) {
        if (!pair.second.defined) {
            undefined.push_back(pair.first);
        }
    }
    
    return undefined;
}
