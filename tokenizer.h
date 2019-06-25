#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdbool.h>
#include "tokens.h"

/// Reads and returns the next token from current file pointer position.
///
/// - parameter file: The file input to be read from
/// - parameter token: The token pointer for a successful Token identification
///
/// - returns: If token retrieval was successful
bool readNextToken(FILE* file, Token* token);

Token* allTokens(FILE* file, int *length);

#endif //TOKENIZER_H
