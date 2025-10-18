#include "lexer.h"
#include <algorithm>
#include <cctype>
#include <sstream>

// Static definitions of instructions with their operand counts
const std::map<std::string, int> Lexer::INSTRUCTIONS = {
    {"ADD", 1}, {"SUB", 1}, {"MUL", 1}, {"DIV", 1},
    {"JMP", 1}, {"JMPN", 1}, {"JMPP", 1}, {"JMPZ", 1},
    {"COPY", 2}, {"LOAD", 1}, {"STORE", 1},
    {"INPUT", 1}, {"OUTPUT", 1}, {"STOP", 0}
};

const std::map<std::string, int> Lexer::DIRECTIVES = {
    {"SPACE", 1}, {"CONST", 1}, {"MACRO", 0}, {"ENDMACRO", 0},
    {"SECAO", 1}, {"SECTION", 1}
};

Lexer::Lexer(const std::vector<std::string>& input_lines) 
    : lines(input_lines), current_line(0), current_pos(0), 
      buffered_token(TokenType::END_OF_FILE, "", 0), has_buffered_token(false) {
    if (!lines.empty()) {
        current_line_text = lines[0];
    }
}

Token Lexer::getNextToken() {
    if (has_buffered_token) {
        has_buffered_token = false;
        return buffered_token;
    }
    return readNextToken();
}

Token Lexer::peekNextToken() {
    if (!has_buffered_token) {
        buffered_token = readNextToken();
        has_buffered_token = true;
    }
    return buffered_token;
}

void Lexer::putBackToken(const Token& token) {
    buffered_token = token;
    has_buffered_token = true;
}

Token Lexer::readNextToken() {
    while (current_line < lines.size()) {
        if (current_pos >= current_line_text.length()) {
            current_line++;
            if (current_line < lines.size()) {
                current_line_text = lines[current_line];
                current_pos = 0;
            }
            continue;
        }
        
        skipWhitespace();
        
        // Skip empty lines
        if (current_pos >= current_line_text.length()) {
            continue;
        }
        
        // Handle comments
        if (current_line_text[current_pos] == ';') {
            current_pos = current_line_text.length();
            continue;
        }
        
        // Handle comma
        if (current_line_text[current_pos] == ',') {
            current_pos++;
            return Token(TokenType::COMMA, ",", current_line + 1);
        }
        
        // Read a word
        std::string word = readWord();
        if (word.empty()) {
            continue;
        }
        
        // Check if it's a label (ends with colon)
        if (current_pos < current_line_text.length() && 
            current_line_text[current_pos] == ':') {
            current_pos++;  // Skip the colon
            
            // Validate label
            if (!isValidLabel(word)) {
                return Token(TokenType::ERROR, "Invalid label: " + word, current_line + 1);
            }
            
            return Token(TokenType::LABEL, word, current_line + 1);
        }
        
        // Check for section directive
        std::string upper_word = toUpper(word);
        if (upper_word == "SECAO" || upper_word == "SECTION") {
            return Token(TokenType::SECTION, upper_word, current_line + 1);
        }
        
        // Check for macro directives
        if (upper_word == "MACRO") {
            return Token(TokenType::MACRO_DEF, upper_word, current_line + 1);
        }
        if (upper_word == "ENDMACRO") {
            return Token(TokenType::MACRO_END, upper_word, current_line + 1);
        }
        
        // Check if it's an instruction
        if (isInstruction(upper_word)) {
            return Token(TokenType::INSTRUCTION, upper_word, current_line + 1);
        }
        
        // Check if it's a directive
        if (isDirective(upper_word)) {
            return Token(TokenType::DIRECTIVE, upper_word, current_line + 1);
        }
        
        // Otherwise, it's an operand
        return Token(TokenType::OPERAND, word, current_line + 1);
    }
    
    return Token(TokenType::END_OF_FILE, "", current_line + 1);
}

bool Lexer::hasMoreTokens() {
    return current_line < lines.size();
}

void Lexer::skipWhitespace() {
    while (current_pos < current_line_text.length() && 
           (current_line_text[current_pos] == ' ' || 
            current_line_text[current_pos] == '\t')) {
        current_pos++;
    }
}

std::string Lexer::readWord() {
    std::string word;
    
    while (current_pos < current_line_text.length()) {
        char c = current_line_text[current_pos];
        
        // Stop at whitespace, comma, colon, or comment
        if (c == ' ' || c == '\t' || c == ',' || c == ':' || c == ';') {
            break;
        }
        
        word += c;
        current_pos++;
    }
    
    return word;
}

bool Lexer::isValidLabel(const std::string& label) {
    if (label.empty()) return false;
    
    // Label must start with letter or underscore
    if (!std::isalpha(label[0]) && label[0] != '_') {
        return false;
    }
    
    // Rest can be letters, numbers, or underscore
    for (size_t i = 1; i < label.length(); i++) {
        if (!std::isalnum(label[i]) && label[i] != '_') {
            return false;
        }
    }
    
    return true;
}

bool Lexer::isInstruction(const std::string& word) {
    return INSTRUCTIONS.find(word) != INSTRUCTIONS.end();
}

bool Lexer::isDirective(const std::string& word) {
    return DIRECTIVES.find(word) != DIRECTIVES.end();
}

std::string Lexer::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}
