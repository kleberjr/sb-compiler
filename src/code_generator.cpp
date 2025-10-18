#include "code_generator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

CodeGenerator::CodeGenerator(const std::vector<Instruction>& insts, SymbolTable& st)
    : instructions(insts), symbol_table(st) {
}

void CodeGenerator::generateIntermediateCode() {
    object_code.clear();
    
    for (const auto& inst : instructions) {
        generateInstructionCode(inst);
    }
}

void CodeGenerator::generateInstructionCode(const Instruction& inst) {
    switch (inst.type) {
        case InstructionType::SPACE: {
            // SPACE directive - add zeros
            int count = 1;
            if (!inst.operands.empty()) {
                // Parse the count if provided
                try {
                    count = std::stoi(inst.operands[0]);
                } catch (...) {
                    count = 1;
                }
            }
            for (int i = 0; i < count; i++) {
                object_code.push_back(0);
            }
            break;
        }
        
        case InstructionType::CONST: {
            // CONST directive - add the constant value
            if (!inst.operands.empty()) {
                int value = resolveOperand(inst.operands[0]);
                object_code.push_back(value);
            } else {
                object_code.push_back(0);
            }
            break;
        }
        
        case InstructionType::STOP: {
            // STOP has no operands
            object_code.push_back(inst.opcode);
            break;
        }
        
        case InstructionType::COPY: {
            // COPY has two operands
            object_code.push_back(inst.opcode);
            if (inst.operands.size() >= 2) {
                int addr1 = resolveOperand(inst.operands[0]);
                int addr2 = resolveOperand(inst.operands[1]);
                object_code.push_back(addr1);
                object_code.push_back(addr2);
            } else {
                object_code.push_back(-1);
                object_code.push_back(-1);
            }
            break;
        }
        
        default: {
            // Most instructions have one operand
            object_code.push_back(inst.opcode);
            if (!inst.operands.empty()) {
                int addr = resolveOperand(inst.operands[0]);
                object_code.push_back(addr);
            } else {
                object_code.push_back(-1);
            }
            break;
        }
    }
}

int CodeGenerator::resolveOperand(const std::string& operand) {
    // Check if it's a number
    if (!operand.empty() && (std::isdigit(operand[0]) || 
        operand[0] == '-' || operand[0] == '+')) {
        
        // Handle hex numbers
        if (operand.find("0X") != std::string::npos || 
            operand.find("0x") != std::string::npos) {
            try {
                return std::stoi(operand, nullptr, 16);
            } catch (...) {
                return 0;
            }
        }
        
        // Handle decimal numbers
        try {
            return std::stoi(operand);
        } catch (...) {
            return 0;
        }
    }
    
    // It's a symbol - look it up in the symbol table
    if (symbol_table.isSymbolDefined(operand)) {
        return symbol_table.getSymbolAddress(operand);
    }
    
    // Symbol not defined - add pending reference
    int current_pos = object_code.size();
    symbol_table.addPendingReference(current_pos, operand, 0);
    return -1;  // Placeholder for pending reference
}

void CodeGenerator::writeIntermediateCode(const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    // Write the intermediate code with annotations for pending references
    for (size_t i = 0; i < object_code.size(); i++) {
        out << object_code[i];
        
        // Check if this position has a pending reference
        bool has_pending = false;
        for (const auto& ref : symbol_table.getPendingReferences()) {
            if (ref.instruction_address == i) {
                out << " ; Pending: " << ref.symbol_name;
                has_pending = true;
                break;
            }
        }
        
        if (i < object_code.size() - 1) {
            out << " ";
        }
    }
    
    out << "\n";
    
    // Add symbol table information
    out << "\n; Symbol Table:\n";
    for (const auto& undefined : symbol_table.getUndefinedSymbols()) {
        out << "; " << undefined << ": UNDEFINED\n";
    }
    
    out.close();
}

void CodeGenerator::generateFinalCode() {
    // First generate intermediate code if not already done
    if (object_code.empty()) {
        generateIntermediateCode();
    }
    
    // Resolve all pending references
    auto resolutions = symbol_table.resolvePendingReferences();
    
    for (const auto& resolution : resolutions) {
        if (resolution.first < object_code.size()) {
            object_code[resolution.first] = resolution.second;
        }
    }
}

void CodeGenerator::writeFinalCode(const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    // Write all integers in a single line, space-separated
    for (size_t i = 0; i < object_code.size(); i++) {
        out << object_code[i];
        if (i < object_code.size() - 1) {
            out << " ";
        }
    }
    
    out << "\n";
    out.close();
}
