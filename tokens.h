#ifndef TOKENS_H
#define TOKENS_H

#define MAX_TOKEN_LENGTH 64

#include "uthash/uthash.h"

typedef enum {
    INVALID = -1,
    IDENTIFIER = 10,
    ARITHMETIC,
    RELATIONAL,
    ATTRIBUTION,
    NUMBER,
    RESERVED_WORD,
    TYPE,
    BOOLEAN,
    LOGIC,
    DELIMITATOR
} TokenType;

typedef enum {
    W_AND = 100,
    W_BEGIN,
    W_BOOLEAN,
    W_DO,
    W_DIV,
    W_ELSE,
    W_END,
    W_FALSE,
    W_FOR,
    W_FROM,
    W_TO,
    W_INCREMENT,
    W_ENDFOR,
    W_IF,
    W_ENDIF,
    W_INTEGER,
    W_NOT,
    W_OR,
    W_PROCEDURE,
    W_PROGRAM,
    W_THEN,
    W_TRUE,
    W_VAR,
    W_WHILE,
    W_ENDWHILE,
    W_WRITE,
    W_READ,
    UNKOWN_WORD
} TokenWord;

// A token contains a type and a value but it can or not contain TokenWord
// it depends whether on token type:
// RESERVED_WORD use TokenWord to match faster
typedef struct Token {
    TokenType type;
    char* value;
    TokenWord word;  
	UT_hash_handle hh;  
} Token;

char* tokenTypeDescription(TokenType type);

char* tokenWordDescription(TokenWord word);

char* tokenDescription(Token token);

#endif //TOKENS_H
