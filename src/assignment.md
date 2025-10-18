Assignment 1 — Basic Software
1. Context and Objective

This assignment aims to consolidate the theoretical concepts of assemblers and compilers covered in the Basic Software course by implementing a simplified compiler in C or C++.

The program must simulate a single-pass assembler/compiler capable of handling:

Macros,

Labels and symbol tables,

Pending references,

Text-based object code generation.

It processes an Assembly file (.asm) and automatically generates three outputs:

.pre → after preprocessing (macro expansion);

.o1 → code with unresolved references;

.o2 → final object code.

2. Execution and Deliverables
💻 Execution
./compiler file.asm

📂 Expected Outputs
File	Content
file.pre	Code after macro expansion
file.o1	Intermediate code with pending references
file.o2	Final compiled object code
🧾 Deliverables

Source code in C or C++ (no external libraries);

README.txt containing:

Team members (up to 3);

Compilation instructions (GCC or Code::Blocks);

Operating system used;

Execution example;

All input/output files must be in the same directory as the executable;

Deadline: within 30 days after October 31, 2025.

3. Compiler Structure and Stages
🧩 Stage 1 — Preprocessing (.pre)

Expands up to two macros per program;

Each macro may receive up to 2 arguments and call another macro;

Supported directives: SPACE and CONST;

EQU and IF are not used;

Output should show code after macro expansion.

💡 Tip: corresponds to the first assembler pass, performing macro replacement and code cleanup.

⚙️ Stage 2 — Single Pass (.o1)

Processes the program in one pass;

Builds:

Symbol table (label → address);

Pending reference list;

Pending references are not resolved yet;

Output shows the intermediate code with pending notes.

💾 Stage 3 — Final Code Generation (.o2)

Resolves all references and replaces labels with addresses;

Removes unnecessary spaces and comments;

SPACE directives are replaced by 0;

Output is a single line of integers, e.g.:

5 9 8 12 2 12 3 12 1 12 14 0

4. Error Handling

The compiler must identify lexical, syntactic, and semantic errors, marking the line number and error type in the .pre file.

Type	Example
Lexical	Label starts with a number or contains invalid character
Syntactic	Invalid number of parameters; two labels on one line
Semantic	Duplicated label; undefined label; nonexistent instruction
5. Recommended Internal Structure
📁 Suggested Modules

lexer.c → lexical analysis and normalization;

macro.c → macro expansion;

parser.c → directive and instruction interpretation;

symbol_table.c → symbol table management;

linker.c → pending resolution and .o2 generation;

main.c → main program orchestration.

🧠 Data Structures

map<string,int> → symbol table;

vector<Pending> → pending references {line, symbol, position};

vector<string> → processed code lines.

6. Syntax Rules and Instructions
Type	Example	Notes
Label	LOOP:	Ends with :
Instruction	ADD VAR1	Case-insensitive
Directive	SPACE 3, CONST 10	Reserve or define values
Comment	; text	Everything after ; is ignored
7. Implementation and Testing Tips

Read the file line by line, removing tabs/spaces;

Identify and store labels;

Expand macros before assembly;

Use placeholders (-1) for unresolved references;

Test using small programs and examples from class slides.

8. Evaluation Criteria
Criterion	Weight
Basic functionality (.pre, .o1, .o2)	30%
Macro expansion correctness	15%
Error handling	15%
Final code accuracy	20%
Organization and clarity	20%
9. Connection to Theoretical Content
Topic	Application in Project
Two-pass assembler	Basis for single-pass design
Symbol tables	Implemented in .o1
Relocation and linking	Implemented in .o2
Macros and directives	Preprocessing phase
Loader and file format	.o2 output
Assembly IA-32 and x64	Syntax foundation
Lexical/syntactic errors	Based on grammar rules