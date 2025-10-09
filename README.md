# SB Compiler - Software Básico

## 📋 Features

- **Three-stage compilation pipeline**
  - `.pre` - Preprocessed code with macro expansion
  - `.o1` - Intermediate code with pending references
  - `.o2` - Final object code
- **Macro support** - Up to 2 macros per program with parameters
- **Symbol table management** with forward reference resolution
- **Comprehensive error detection** (lexical, syntactic, and semantic)
- **Support for hexadecimal numbers** (e.g., `0xBB`, `-0XFF`)

## 🛠️ Building

### Prerequisites
- C++ compiler with C++11 support (g++ or clang++)
- Make

### Compilation

```bash
# Build the compiler and simulator
make

# Build only the compiler
make compiler

# Build only the simulator
make simulador

# Clean build artifacts
make clean
```

## 🚀 Usage

### Basic Usage

```bash
# Compile an assembly file
./compiler source.asm

# This generates:
#   source.pre - Preprocessed code
#   source.o1  - Intermediate code
#   source.o2  - Final object code
```

### Running with Simulator

```bash
# Run the compiled program
./simulador program.o2

# With input
echo "5 3" | ./simulador program.o2

# With trace mode
./simulador program.o2 --trace
```

## 📝 Assembly Language

### Instructions

| Instruction | Opcode | Size | Description |
|------------|--------|------|-------------|
| ADD op | 1 | 2 | ACC = ACC + mem[op] |
| SUB op | 2 | 2 | ACC = ACC - mem[op] |
| MUL op | 3 | 2 | ACC = ACC * mem[op] |
| DIV op | 4 | 2 | ACC = ACC / mem[op] |
| JMP op | 5 | 2 | PC = op |
| JMPN op | 6 | 2 | if (ACC < 0) PC = op |
| JMPP op | 7 | 2 | if (ACC > 0) PC = op |
| JMPZ op | 8 | 2 | if (ACC == 0) PC = op |
| COPY a b | 9 | 3 | mem[b] = mem[a] |
| LOAD op | 10 | 2 | ACC = mem[op] |
| STORE op | 11 | 2 | mem[op] = ACC |
| INPUT op | 12 | 2 | Read int from stdin → mem[op] |
| OUTPUT op | 13 | 2 | Print mem[op] |
| STOP | 14 | 1 | Halt execution |

### Directives

- `SPACE [n]` - Reserve n memory words (default: 1)
- `CONST value` - Define constant value
- `SECAO TEXTO/DADOS` - Section declaration
- `MACRO/ENDMACRO` - Macro definition

### Example Program

```asm
; Simple addition program
SECAO TEXTO
    INPUT A
    INPUT B
    LOAD A
    ADD B
    STORE C
    OUTPUT C
    STOP

SECAO DADOS
A: SPACE
B: SPACE
C: SPACE
```

### Macro Example

```asm
SWAP: MACRO X, Y
    LOAD X
    STORE TEMP
    LOAD Y
    STORE X
    LOAD TEMP
    STORE Y
ENDMACRO

SECAO TEXTO
    INPUT NUM1
    INPUT NUM2
    SWAP NUM1, NUM2  ; Expands to 6 instructions
    OUTPUT NUM1
    OUTPUT NUM2
    STOP

SECAO DADOS
NUM1: SPACE
NUM2: SPACE
TEMP: SPACE
```

## 🧪 Testing

```bash
# Run basic test
make test

# Test macro expansion
make test-macro

# Run complete test with simulator
make test-complete
```

## 📁 Project Structure

```
sb-compiler/
├── Makefile           # Build configuration
├── README.md          # This file
├── compiler           # Main executable
├── simulador          # Machine simulator
├── obj/              # Object files (generated)
└── src/              # Source code
    ├── compiler.cpp       # Main entry point
    ├── lexer.cpp/h       # Lexical analysis
    ├── preprocessor.cpp/h # Macro expansion
    ├── parser.cpp/h      # Syntax analysis
    ├── symbol_table.cpp/h # Symbol management
    ├── code_generator.cpp/h # Code generation
    └── *.asm            # Test files
```

## 🎯 Implementation Details

### Compilation Process

1. **Lexical Analysis**: Tokenizes input, removes comments, identifies labels
2. **Preprocessing**: Expands macros, processes directives
3. **Parsing**: Builds symbol table, validates syntax
4. **Code Generation**: Generates intermediate code with pending references
5. **Linking**: Resolves references, produces final object code

### Error Detection

- **Lexical Errors**: Invalid labels, malformed tokens
- **Syntactic Errors**: Wrong operand count, invalid instructions
- **Semantic Errors**: Undefined symbols, duplicate labels

## 📊 Output Format

### .pre File
Preprocessed assembly with macros expanded and comments removed.

### .o1 File
Intermediate code with pending references marked as -1, includes symbol table annotations.

### .o2 File
Final object code as a single line of space-separated integers ready for execution.

## 👥 Team

Kleber Rodrigues da Costa Júnior - 200053680
Joao Vitor Maia Ferreira - 190110007

## 📄 License

This project was developed as part of the Software Básico course at UnB (University of Brasília).

## 🔧 Troubleshooting

### Common Issues

1. **Undefined symbols**: Check that all labels are defined in the DATA section
2. **Duplicate labels**: Ensure each label appears only once
3. **Invalid operand count**: COPY needs 2 operands, STOP needs 0, others need 1
4. **Macro errors**: Maximum 2 macros per program, max 2 parameters each

### Debug Mode

To see detailed compilation steps:
```bash
./compiler source.asm 2>&1 | less
```
