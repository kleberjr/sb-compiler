#include "parser.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

Parser::Parser(const std::vector<std::string>& lines) 
    : lexer(lines), current_address(0), in_text_section(false), in_data_section(false) {
}

void Parser::parse() {
    while (lexer.hasMoreTokens()) {
        Token token = lexer.getNextToken();
        
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
        
        switch (token.type) {
            case TokenType::LABEL:
                parseLabel(token);
                break;
                
            case TokenType::INSTRUCTION:
                parseInstruction(token);
                break;
                
            case TokenType::DIRECTIVE:
                parseDirective(token);
                break;
                
            case TokenType::SECTION:
                parseSection(token);
                break;
                
            case TokenType::ERROR:
                errors.push_back(ParseError(ParseError::LEXICAL, token.value, token.line_number));
                break;
                
            case TokenType::COMMA:
                // Skip commas at the top level
                break;
                
            case TokenType::OPERAND:
                // Operand at top level might be an error
                errors.push_back(ParseError(ParseError::SYNTACTIC, 
                    "Unexpected operand at top level: " + token.value, token.line_number));
                break;
                
            default:
                // Unexpected token
                errors.push_back(ParseError(ParseError::SYNTACTIC, 
                    "Unexpected token: " + token.value, token.line_number));
                break;
        }
    }
    
    // Check for undefined symbols
    std::vector<std::string> undefined = symbol_table.getUndefinedSymbols();
    for (const auto& sym : undefined) {
        errors.push_back(ParseError(ParseError::SEMANTIC,
            "Undefined symbol: " + sym, 0));
    }
}

void Parser::parseLabel(Token& token) {
    std::string label_name = token.value;
    
    // Check if label already exists
    if (symbol_table.isSymbolDefined(label_name)) {
        errors.push_back(ParseError(ParseError::SEMANTIC,
            "Duplicate label: " + label_name, token.line_number));
        return;
    }
    
    // Add label to symbol table
    symbol_table.defineSymbol(label_name, current_address);
}

void Parser::parseInstruction(Token& token) {
    Instruction inst;
    inst.type = getInstructionType(token.value);
    inst.line_number = token.line_number;
    inst.address = current_address;
    
    if (inst.type == InstructionType::INVALID) {
        errors.push_back(ParseError(ParseError::SEMANTIC,
            "Invalid instruction: " + token.value, token.line_number));
        return;
    }
    
    inst.opcode = getOpcode(inst.type);
    inst.size = getInstructionSize(inst.type);
    
    // Get expected operand count
    int expected_operands = 0;
    switch (inst.type) {
        case InstructionType::COPY:
            expected_operands = 2;
            break;
        case InstructionType::STOP:
            expected_operands = 0;
            break;
        default:
            expected_operands = 1;
            break;
    }
    
    // Get operands
    for (int i = 0; i < expected_operands; i++) {
        if (i > 0) {
            // Expect a comma between operands
            if (lexer.hasMoreTokens()) {
                Token comma = lexer.getNextToken();
                if (comma.type != TokenType::COMMA) {
                    errors.push_back(ParseError(ParseError::SYNTACTIC,
                        "Expected comma between operands", token.line_number));
                    return;
                }
            }
        }
        
        if (lexer.hasMoreTokens()) {
            Token operand = lexer.getNextToken();
            if (operand.type == TokenType::OPERAND) {
                inst.operands.push_back(operand.value);
            } else {
                errors.push_back(ParseError(ParseError::SYNTACTIC,
                    "Expected operand for " + token.value, token.line_number));
                return;
            }
        } else {
            errors.push_back(ParseError(ParseError::SYNTACTIC,
                "Missing operand for " + token.value, token.line_number));
            return;
        }
    }
    
    instructions.push_back(inst);
    current_address += inst.size;
}

void Parser::parseDirective(Token& token) {
    Instruction inst;
    inst.line_number = token.line_number;
    inst.address = current_address;
    
    if (token.value == "SPACE") {
        inst.type = InstructionType::SPACE;
        inst.size = 1;  // Default size
        
        // Check for optional operand
        if (lexer.hasMoreTokens()) {
            Token next = lexer.peekNextToken();
            if (next.type == TokenType::OPERAND) {
                lexer.getNextToken(); // Consume the operand
                if (isNumber(next.value)) {
                    int count = parseNumber(next.value);
                    inst.size = count;
                    inst.operands.push_back(next.value);
                } else {
                    errors.push_back(ParseError(ParseError::SYNTACTIC,
                        "SPACE requires numeric operand", token.line_number));
                }
            }
            // If it's not an operand, leave it for the next iteration
        }
    } else if (token.value == "CONST") {
        inst.type = InstructionType::CONST;
        inst.size = 1;
        
        // CONST requires an operand
        if (lexer.hasMoreTokens()) {
            Token next = lexer.getNextToken();
            if (next.type == TokenType::OPERAND) {
                inst.operands.push_back(next.value);
            } else {
                errors.push_back(ParseError(ParseError::SYNTACTIC,
                    "CONST requires an operand", token.line_number));
                // Put the token back since it's not an operand
                lexer.putBackToken(next);
            }
        } else {
            errors.push_back(ParseError(ParseError::SYNTACTIC,
                "CONST requires an operand", token.line_number));
        }
    }
    
    instructions.push_back(inst);
    current_address += inst.size;
}

void Parser::parseSection(Token& token) {
    Token next = lexer.getNextToken();
    
    if (next.type != TokenType::OPERAND) {
        errors.push_back(ParseError(ParseError::SYNTACTIC,
            "Section directive requires a name", token.line_number));
        return;
    }
    
    std::string section_name = next.value;
    std::transform(section_name.begin(), section_name.end(), 
                   section_name.begin(), ::toupper);
    
    if (section_name == "TEXTO" || section_name == "TEXT") {
        in_text_section = true;
        in_data_section = false;
    } else if (section_name == "DADOS" || section_name == "DATA") {
        in_text_section = false;
        in_data_section = true;
    } else {
        errors.push_back(ParseError(ParseError::SEMANTIC,
            "Unknown section: " + section_name, token.line_number));
    }
}

InstructionType Parser::getInstructionType(const std::string& name) {
    if (name == "ADD") return InstructionType::ADD;
    if (name == "SUB") return InstructionType::SUB;
    if (name == "MUL") return InstructionType::MUL;
    if (name == "DIV") return InstructionType::DIV;
    if (name == "JMP") return InstructionType::JMP;
    if (name == "JMPN") return InstructionType::JMPN;
    if (name == "JMPP") return InstructionType::JMPP;
    if (name == "JMPZ") return InstructionType::JMPZ;
    if (name == "COPY") return InstructionType::COPY;
    if (name == "LOAD") return InstructionType::LOAD;
    if (name == "STORE") return InstructionType::STORE;
    if (name == "INPUT") return InstructionType::INPUT;
    if (name == "OUTPUT") return InstructionType::OUTPUT;
    if (name == "STOP") return InstructionType::STOP;
    return InstructionType::INVALID;
}

int Parser::getOpcode(InstructionType type) {
    switch (type) {
        case InstructionType::ADD: return 1;
        case InstructionType::SUB: return 2;
        case InstructionType::MUL: return 3;
        case InstructionType::DIV: return 4;
        case InstructionType::JMP: return 5;
        case InstructionType::JMPN: return 6;
        case InstructionType::JMPP: return 7;
        case InstructionType::JMPZ: return 8;
        case InstructionType::COPY: return 9;
        case InstructionType::LOAD: return 10;
        case InstructionType::STORE: return 11;
        case InstructionType::INPUT: return 12;
        case InstructionType::OUTPUT: return 13;
        case InstructionType::STOP: return 14;
        default: return -1;
    }
}

int Parser::getInstructionSize(InstructionType type) {
    switch (type) {
        case InstructionType::COPY: return 3;
        case InstructionType::STOP: return 1;
        case InstructionType::SPACE: return 1;  // Can be overridden
        case InstructionType::CONST: return 1;
        default: return 2;
    }
}

bool Parser::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    
    // Handle negative numbers
    if (str[0] == '-' || str[0] == '+') {
        if (str.length() == 1) return false;
        start = 1;
    }
    
    // Handle hex numbers
    if (str.substr(start, 2) == "0X" || str.substr(start, 2) == "0x") {
        start += 2;
        for (size_t i = start; i < str.length(); i++) {
            if (!std::isxdigit(str[i])) return false;
        }
        return true;
    }
    
    // Handle decimal numbers
    for (size_t i = start; i < str.length(); i++) {
        if (!std::isdigit(str[i])) return false;
    }
    
    return true;
}

int Parser::parseNumber(const std::string& str) {
    if (str.substr(0, 2) == "0X" || str.substr(0, 2) == "0x" ||
        str.substr(0, 3) == "-0X" || str.substr(0, 3) == "-0x") {
        return std::stoi(str, nullptr, 16);
    }
    return std::stoi(str);
}

void Parser::printErrors() const {
    for (const auto& error : errors) {
        std::string type_str;
        switch (error.type) {
            case ParseError::LEXICAL:
                type_str = "Lexical";
                break;
            case ParseError::SYNTACTIC:
                type_str = "Syntactic";
                break;
            case ParseError::SEMANTIC:
                type_str = "Semantic";
                break;
        }
        
        std::cerr << "Error (" << type_str << ") at line " 
                  << error.line_number << ": " << error.message << "\n";
    }
}
