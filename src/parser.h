#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "lexer.h"
#include "symbol_table.h"

enum class InstructionType {
    ADD, SUB, MUL, DIV,
    JMP, JMPN, JMPP, JMPZ,
    COPY, LOAD, STORE,
    INPUT, OUTPUT, STOP,
    SPACE, CONST,
    INVALID
};

struct Instruction {
    InstructionType type;
    int opcode;
    std::vector<std::string> operands;
    int size;  // Size in memory words
    int address;  // Memory address
    int line_number;
    
    Instruction() : type(InstructionType::INVALID), opcode(-1), size(0), address(0), line_number(0) {}
};

struct ParseError {
    enum Type { LEXICAL, SYNTACTIC, SEMANTIC };
    Type type;
    std::string message;
    int line_number;
    
    ParseError(Type t, const std::string& msg, int line) 
        : type(t), message(msg), line_number(line) {}
};

class Parser {
private:
    Lexer lexer;
    SymbolTable symbol_table;
    std::vector<Instruction> instructions;
    std::vector<ParseError> errors;
    int current_address;
    bool in_text_section;
    bool in_data_section;
    
    // Helper functions
    InstructionType getInstructionType(const std::string& name);
    int getOpcode(InstructionType type);
    int getInstructionSize(InstructionType type);
    bool validateOperandCount(InstructionType type, int count);
    void parseInstruction(Token& token);
    void parseDirective(Token& token);
    void parseLabel(Token& token);
    void parseSection(Token& token);
    bool isNumber(const std::string& str);
    int parseNumber(const std::string& str);
    
public:
    Parser(const std::vector<std::string>& lines);
    void parse();
    
    const std::vector<Instruction>& getInstructions() const { return instructions; }
    const std::vector<ParseError>& getErrors() const { return errors; }
    SymbolTable& getSymbolTable() { return symbol_table; }
    bool hasErrors() const { return !errors.empty(); }
    
    void printErrors() const;
};

#endif // PARSER_H
