#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <vector>
#include <map>

struct Macro {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> body;
    
    Macro() {}
    Macro(const std::string& n) : name(n) {}
};

class Preprocessor {
private:
    std::vector<std::string> input_lines;
    std::vector<std::string> output_lines;
    std::map<std::string, Macro> macros;
    std::map<std::string, int> constants;  // For EQU directives (if needed)
    
    // Helper functions
    std::string removeComments(const std::string& line);
    std::string trim(const std::string& str);
    std::string toUpper(const std::string& str);
    bool isMacroDefinition(const std::string& line);
    bool isMacroCall(const std::string& line);
    std::vector<std::string> expandMacro(const std::string& line, const Macro& macro);
    std::vector<std::string> splitParameters(const std::string& params);
    std::string replaceParameters(const std::string& line, 
                                   const std::vector<std::string>& params,
                                   const std::vector<std::string>& args);
    
public:
    Preprocessor(const std::vector<std::string>& lines);
    std::vector<std::string> preprocess();
    void writeToFile(const std::string& filename);
};

#endif // PREPROCESSOR_H
