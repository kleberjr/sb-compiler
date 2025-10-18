================================================================================
                         SB COMPILER - SOFTWARE BÁSICO
================================================================================

EQUIPE:
-------
[Adicionar nomes dos membros da equipe aqui - até 3 pessoas]

SISTEMA OPERACIONAL:
--------------------
macOS (Darwin) / Linux / Windows com WSL

COMPILAÇÃO:
-----------
Para compilar o projeto, execute:

    make clean
    make

Ou diretamente com g++:

    g++ -std=c++11 -Wall -O2 -o compiler compiler.cpp lexer.cpp preprocessor.cpp \
        parser.cpp symbol_table.cpp code_generator.cpp

EXECUÇÃO:
---------
./compiler arquivo.asm

O compilador irá gerar automaticamente três arquivos:
- arquivo.pre  : Código após expansão de macros
- arquivo.o1   : Código intermediário com referências pendentes
- arquivo.o2   : Código objeto final

EXEMPLO DE USO:
---------------
$ ./compiler teste.asm
Reading teste.asm...
Preprocessing...
Generated teste.pre
Parsing...
Generating intermediate code...
Generated teste.o1
Generating final object code...
Generated teste.o2

Compilation successful!
Output files:
  teste.pre - Preprocessed code
  teste.o1 - Intermediate code
  teste.o2 - Final object code

TESTE COM SIMULADOR:
--------------------
$ echo "5 3" | ./simulador teste.o2
8

ESTRUTURA DO PROJETO:
---------------------
- compiler.cpp       : Programa principal e orquestração
- lexer.cpp/h       : Análise léxica e tokenização
- preprocessor.cpp/h : Expansão de macros e pré-processamento
- parser.cpp/h      : Análise sintática e construção da tabela de símbolos
- symbol_table.cpp/h : Gerenciamento da tabela de símbolos
- code_generator.cpp/h : Geração de código objeto
- Makefile          : Script de compilação

FUNCIONALIDADES IMPLEMENTADAS:
-------------------------------
✓ Processamento de macros (até 2 por programa)
✓ Macros com parâmetros (até 2 parâmetros)
✓ Diretivas SPACE e CONST
✓ Tabela de símbolos
✓ Resolução de referências pendentes
✓ Geração de código objeto em formato texto
✓ Detecção de erros léxicos, sintáticos e semânticos
✓ Suporte a números hexadecimais

INSTRUÇÕES SUPORTADAS:
----------------------
ADD, SUB, MUL, DIV, JMP, JMPN, JMPP, JMPZ, COPY, LOAD, STORE, INPUT, OUTPUT, STOP

DIRETIVAS SUPORTADAS:
---------------------
SPACE [n], CONST valor, SECAO TEXTO/DADOS

================================================================================
