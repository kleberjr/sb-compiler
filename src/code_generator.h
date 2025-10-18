#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <string>
#include <vector>
#include "parser.h"
#include "symbol_table.h"

class CodeGenerator {
private:
    const std::vector<Instruction>& instructions;
    SymbolTable& symbol_table;
    std::vector<int> object_code;
    
    // Helper functions
    int resolveOperand(const std::string& operand);
    void generateInstructionCode(const Instruction& inst);
    
public:
    CodeGenerator(const std::vector<Instruction>& insts, SymbolTable& st);
    
    // Generate intermediate code with pending references (.o1)
    void generateIntermediateCode();
    void writeIntermediateCode(const std::string& filename);
    
    // Generate final object code (.o2)
    void generateFinalCode();
    void writeFinalCode(const std::string& filename);
    
    const std::vector<int>& getObjectCode() const { return object_code; }
};

#endif // CODE_GENERATOR_H
