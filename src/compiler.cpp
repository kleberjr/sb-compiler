#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "lexer.h"
#include "preprocessor.h"
#include "parser.h"
#include "code_generator.h"

std::vector<std::string> readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }
    
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    file.close();
    return lines;
}

std::string getBaseName(const std::string& filename) {
    size_t lastDot = filename.find_last_of('.');
    if (lastDot != std::string::npos) {
        return filename.substr(0, lastDot);
    }
    return filename;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file.asm\n";
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string base_name = getBaseName(input_file);
    
    try {
        // Step 1: Read the input file
        std::cout << "Reading " << input_file << "...\n";
        std::vector<std::string> lines = readFile(input_file);
        
        // Step 2: Preprocessing (macro expansion)
        std::cout << "Preprocessing...\n";
        Preprocessor preprocessor(lines);
        std::vector<std::string> preprocessed_lines = preprocessor.preprocess();
        
        // Write .pre file
        std::string pre_file = base_name + ".pre";
        preprocessor.writeToFile(pre_file);
        std::cout << "Generated " << pre_file << "\n";
        
        // Step 3: Parse the preprocessed code
        std::cout << "Parsing...\n";
        Parser parser(preprocessed_lines);
        parser.parse();
        
        // Check for errors
        if (parser.hasErrors()) {
            std::cerr << "\nCompilation errors found:\n";
            parser.printErrors();
            
            // Still write the .pre file with error annotations
            std::ofstream pre_out(pre_file, std::ios::app);
            pre_out << "\n; ERRORS:\n";
            for (const auto& error : parser.getErrors()) {
                pre_out << "; Line " << error.line_number << ": " << error.message << "\n";
            }
            pre_out.close();
            
            return 1;
        }
        
        // Step 4: Generate intermediate code (.o1)
        std::cout << "Generating intermediate code...\n";
        CodeGenerator generator(parser.getInstructions(), parser.getSymbolTable());
        generator.generateIntermediateCode();
        
        // Write .o1 file
        std::string o1_file = base_name + ".o1";
        generator.writeIntermediateCode(o1_file);
        std::cout << "Generated " << o1_file << "\n";
        
        // Step 5: Generate final code (.o2)
        std::cout << "Generating final object code...\n";
        generator.generateFinalCode();
        
        // Write .o2 file
        std::string o2_file = base_name + ".o2";
        generator.writeFinalCode(o2_file);
        std::cout << "Generated " << o2_file << "\n";
        
        // Check for unresolved symbols
        std::vector<std::string> undefined = parser.getSymbolTable().getUndefinedSymbols();
        if (!undefined.empty()) {
            std::cerr << "\nWarning: Unresolved symbols:\n";
            for (const auto& sym : undefined) {
                std::cerr << "  " << sym << "\n";
            }
        }
        
        std::cout << "\nCompilation successful!\n";
        std::cout << "Output files:\n";
        std::cout << "  " << pre_file << " - Preprocessed code\n";
        std::cout << "  " << o1_file << " - Intermediate code\n";
        std::cout << "  " << o2_file << " - Final object code\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
