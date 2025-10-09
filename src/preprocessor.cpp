#include "preprocessor.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cctype>

Preprocessor::Preprocessor(const std::vector<std::string>& lines) 
    : input_lines(lines) {
}

std::vector<std::string> Preprocessor::preprocess() {
    output_lines.clear();
    
    // First pass: collect macro definitions
    bool in_macro = false;
    Macro current_macro;
    
    for (size_t i = 0; i < input_lines.size(); i++) {
        std::string line = removeComments(input_lines[i]);
        line = trim(line);
        
        if (line.empty()) continue;
        
        // Check for macro definition
        if (isMacroDefinition(line)) {
            in_macro = true;
            // Parse macro name and parameters
            size_t pos = line.find("MACRO");
            if (pos == std::string::npos) {
                pos = line.find("macro");
            }
            
            // Get the label before MACRO
            std::string before_macro = line.substr(0, pos);
            before_macro = trim(before_macro);
            
            // Remove colon if present
            if (!before_macro.empty() && before_macro.back() == ':') {
                before_macro.pop_back();
            }
            
            current_macro = Macro(trim(before_macro));
            
            // Check for parameters after MACRO
            std::string after_macro = line.substr(pos + 5); // Skip "MACRO"
            after_macro = trim(after_macro);
            
            if (!after_macro.empty()) {
                current_macro.parameters = splitParameters(after_macro);
            }
            
            continue;
        }
        
        // Check for end of macro
        if (in_macro && toUpper(line) == "ENDMACRO") {
            macros[current_macro.name] = current_macro;
            in_macro = false;
            current_macro = Macro();
            continue;
        }
        
        // If inside macro, add to body
        if (in_macro) {
            current_macro.body.push_back(line);
            continue;
        }
    }
    
    // Second pass: expand macros and process directives
    for (size_t i = 0; i < input_lines.size(); i++) {
        std::string line = removeComments(input_lines[i]);
        line = trim(line);
        
        if (line.empty()) continue;
        
        // Skip macro definitions in second pass
        if (isMacroDefinition(line)) {
            // Skip until ENDMACRO
            while (i < input_lines.size() && toUpper(trim(input_lines[i])) != "ENDMACRO") {
                i++;
            }
            continue;
        }
        
        // Check for macro call
        bool expanded = false;
        if (isMacroCall(line)) {
            // Parse macro call
            std::istringstream iss(line);
            std::string macro_name;
            iss >> macro_name;
            
            // Remove colon if it's a label
            if (!macro_name.empty() && macro_name.back() == ':') {
                macro_name.pop_back();
            }
            
            if (macros.find(macro_name) != macros.end()) {
                std::vector<std::string> expanded_lines = expandMacro(line, macros[macro_name]);
                for (const auto& exp_line : expanded_lines) {
                    output_lines.push_back(exp_line);
                }
                expanded = true;
            }
        }
        
        if (!expanded) {
            output_lines.push_back(line);
        }
    }
    
    return output_lines;
}

void Preprocessor::writeToFile(const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    for (const auto& line : output_lines) {
        out << line << "\n";
    }
    
    out.close();
}

std::string Preprocessor::removeComments(const std::string& line) {
    size_t comment_pos = line.find(';');
    if (comment_pos != std::string::npos) {
        return line.substr(0, comment_pos);
    }
    return line;
}

std::string Preprocessor::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string Preprocessor::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Preprocessor::isMacroDefinition(const std::string& line) {
    std::string upper_line = toUpper(line);
    return upper_line.find("MACRO") != std::string::npos && 
           upper_line.find("ENDMACRO") == std::string::npos;
}

bool Preprocessor::isMacroCall(const std::string& line) {
    // A macro call is a line that starts with a macro name
    std::istringstream iss(line);
    std::string first_word;
    iss >> first_word;
    
    // Remove colon if present
    if (!first_word.empty() && first_word.back() == ':') {
        first_word.pop_back();
    }
    
    return macros.find(first_word) != macros.end();
}

std::vector<std::string> Preprocessor::expandMacro(const std::string& line, const Macro& macro) {
    std::vector<std::string> result;
    
    // Parse the macro call to get arguments
    std::istringstream iss(line);
    std::string macro_name;
    iss >> macro_name;
    
    // Remove colon if present
    if (!macro_name.empty() && macro_name.back() == ':') {
        macro_name.pop_back();
    }
    
    // Get the rest as arguments
    std::string args_str;
    std::getline(iss, args_str);
    args_str = trim(args_str);
    
    std::vector<std::string> args;
    if (!args_str.empty()) {
        args = splitParameters(args_str);
    }
    
    // Expand macro body with parameter substitution
    for (const auto& body_line : macro.body) {
        std::string expanded_line = replaceParameters(body_line, macro.parameters, args);
        result.push_back(expanded_line);
    }
    
    return result;
}

std::vector<std::string> Preprocessor::splitParameters(const std::string& params) {
    std::vector<std::string> result;
    std::string current;
    
    for (char c : params) {
        if (c == ',') {
            if (!current.empty()) {
                result.push_back(trim(current));
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        result.push_back(trim(current));
    }
    
    return result;
}

std::string Preprocessor::replaceParameters(const std::string& line,
                                           const std::vector<std::string>& params,
                                           const std::vector<std::string>& args) {
    std::string result = line;
    
    for (size_t i = 0; i < params.size() && i < args.size(); i++) {
        // Replace all occurrences of the parameter with the argument
        size_t pos = 0;
        while ((pos = result.find(params[i], pos)) != std::string::npos) {
            // Check if it's a whole word (not part of another word)
            bool is_word_boundary = true;
            
            if (pos > 0) {
                char prev = result[pos - 1];
                if (std::isalnum(prev) || prev == '_') {
                    is_word_boundary = false;
                }
            }
            
            if (is_word_boundary && pos + params[i].length() < result.length()) {
                char next = result[pos + params[i].length()];
                if (std::isalnum(next) || next == '_') {
                    is_word_boundary = false;
                }
            }
            
            if (is_word_boundary) {
                result.replace(pos, params[i].length(), args[i]);
                pos += args[i].length();
            } else {
                pos++;
            }
        }
    }
    
    return result;
}
