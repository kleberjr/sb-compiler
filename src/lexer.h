#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>

enum class TokenType {
    LABEL,
    INSTRUCTION,
    DIRECTIVE,
    OPERAND,
    SECTION,
    MACRO_DEF,
    MACRO_END,
    COMMA,
    COLON,
    END_OF_FILE,
    ERROR
};

struct Token {
    TokenType type;
    std::string value;
    int line_number;
    
    Token(TokenType t, const std::string& v, int line) 
        : type(t), value(v), line_number(line) {}
};

class Lexer {
private:
    std::vector<std::string> lines;
    int current_line;
    std::string current_line_text;
    size_t current_pos;
    
    // For token buffering
    Token buffered_token;
    bool has_buffered_token;
    
    // Helper functions
    void skipWhitespace();
    std::string readWord();
    bool isValidLabel(const std::string& label);
    bool isInstruction(const std::string& word);
    bool isDirective(const std::string& word);
    std::string toUpper(const std::string& str);
    Token readNextToken();
    
public:
    Lexer(const std::vector<std::string>& input_lines);
    Token getNextToken();
    Token peekNextToken();
    void putBackToken(const Token& token);
    bool hasMoreTokens();
    int getCurrentLine() const { return current_line + 1; }
    std::string getCurrentLineText() const { return current_line_text; }
    
    // Static instruction and directive sets
    static const std::map<std::string, int> INSTRUCTIONS;
    static const std::map<std::string, int> DIRECTIVES;
};

#endif // LEXER_H
