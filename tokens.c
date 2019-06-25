#include <stdio.h>
#include "tokens.h"

char* tokenTypeDescription(TokenType type) {
    switch (type) {
        case INVALID: return "INVALID";
        case IDENTIFIER: return "IDENTIFIER";
        case ARITHMETIC: return "ARITHMETIC";
        case RELATIONAL: return "RELATIONAL";
        case ATTRIBUTION: return "ATTRIBUTION";
        case NUMBER: return "NUMBER";
        case TYPE: return "TYPE";
        case BOOLEAN: return "BOOLEAN";
        case LOGIC: return "LOGIC";
        case RESERVED_WORD: return "RESERVED_WORD";
        case DELIMITATOR: return "DELIMITATOR";
        default: return "UNKNOWN";
    }
}

char* tokenWordDescription(TokenWord word) {
    switch (word) {
        case W_AND: return "e";
        case W_BEGIN: return "inicio";
        case W_BOOLEAN: return "logico";
        case W_DO: return "faca";
        case W_DIV: return "divisao";
        case W_ELSE: return "senao";
        case W_END: return "fimalgoritmo";
        case W_FALSE: return "falso";
        case W_FOR: return "para";
        case W_FROM: return "de";
        case W_TO: return "ate";
        case W_INCREMENT: return "passo";
        case W_ENDFOR: return "fimpara";
        case W_IF: return "se";
        case W_ENDIF: return "fimse";
        case W_INTEGER: return "inteiro";
        case W_NOT: return "nao";
        case W_OR: return "ou";
        case W_PROGRAM: return "algoritmo";
        case W_THEN: return "entao";
        case W_TRUE: return "verdadeiro";
        case W_VAR: return "VAR";
        case W_WHILE: return "enquanto";
        case W_ENDWHILE: return "fimenquanto";
        case W_WRITE: return "escreva";
        case W_READ: return "leia";
        default: return "UNKNOWN";
    }
}

char* tokenDescription(Token token) {
    char* typeDescription = tokenTypeDescription(token.type);
    char* description;

    if (token.type == RESERVED_WORD) {
        char* wordDescription = tokenWordDescription(token.word);
        asprintf(&description, "<%s %s: \"%s\">",
                 typeDescription,
                 wordDescription,
                 token.value);
    } else {
        asprintf(&description, "<%s: \"%s\">",
                 typeDescription,
                 token.value);
    }

    return description;
}
