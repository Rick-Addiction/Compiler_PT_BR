#ifndef SINTAX_H
#define SINTAX_H

#include <stdio.h>
#include <stdbool.h>
#include "tokens.h"


typedef struct Node {
	struct Symbols *symbols;
	struct Node *next;
} Node;

typedef struct Symbols {
    const char *varName;
    TokenWord varType;
    UT_hash_handle hh;
} Symbols;

/// Verifies if the set of tokens from a program makes it valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
///
/// - returns: Boolean value indicating if the program is valid.
bool isValidProgram(Token* tokens, int length);

#endif // SINTAX_H
