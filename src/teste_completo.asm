; Teste completo do compilador
; Demonstra macros, labels, instruções e diretivas

INCREMENT: MACRO VAR
    LOAD VAR
    ADD ONE
    STORE VAR
ENDMACRO

SECAO TEXTO
START:  INPUT N         ; Lê um número
        LOAD N
        JMPZ FIM        ; Se zero, termina
        INCREMENT N     ; Incrementa N (macro)
LOOP:   LOAD N
        SUB ONE
        STORE N
        OUTPUT N
        JMPZ FIM
        JMP LOOP
FIM:    LOAD RESULT
        OUTPUT RESULT
        STOP

SECAO DADOS
N:      SPACE
ONE:    CONST 1
RESULT: CONST 42
TEMP:   SPACE 2
