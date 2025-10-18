SECAO TEXTO
    INPUT A
    ADD B       ; B não definido ainda - referência pendente
    JMP LOOP    ; LOOP não existe - erro
    STORE A
LOOP: ADD A     ; Label duplicado - erro
    OUTPUT A
    STOP
LOOP: SUB A     ; Label duplicado - erro
SECAO DADOS
A: SPACE
B: CONST 10
