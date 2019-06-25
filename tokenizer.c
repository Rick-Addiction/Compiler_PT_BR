#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include "utils.h"
#include "tokens.h"
#include "tokenizer.h"
#include "tokenRecognizer.h"

/// Ignores sequential whitespaces if any
///
/// - parameter file: The file to read
/// - parameter current: The first character after the ignored stream
void ignoreWhitespace(FILE* file, char* current) {
    bool skipped = isWhitespace(*current);
    while(isWhitespace(*current))
        *current = fgetc(file);

    if (skipped)
        fseek(file, -1, SEEK_CUR);
}

/// Tries to match commentary block identifiers ('/*', '*/') and ignores
/// anything between them (them included)
///
/// - parameter file: The file to read
/// - parameter current: The first character after the commentary blocks
void ignoreCommentary(FILE* file, char* current) {
    switch (*current) {
        case '/':
            *current = fgetc(file);
            if (*current == '/')
                goto ignoreloop;
            return;
        default: return;
    }

    ignoreloop:
    *current = fgetc(file);
    switch (*current) {
        case '\n':
            *current = fpeekc(file);
            return;
        case EOF: return;
        default: goto ignoreloop;
    }
}

bool readNextToken(FILE* file, Token* token) {
    char* buffer = (char*)calloc(MAX_TOKEN_LENGTH, sizeof(char));
    char current = fpeekc(file);
    ignoreloop:
    ignoreWhitespace(file, &current);
    if (current == '/') {
        ignoreCommentary(file, &current);
        goto ignoreloop;
    }
    if (current == EOF)
        return false;

    if (isDigit(current))
        *token = validateNumber(file, buffer, 0);
    else if (isInSet(current, reservedLetterSet))
        *token = validateReservedWordToken(file, buffer, 0);
    else if (isInSet(current, operatorLetterSet))
        *token = validateOperatorToken(file, buffer, 0);
    else if (isDelimitator(current))
        *token = validateDelimitatorToken(file, buffer, 0);
    else if (isValidIdentifier(current))
        *token = validateIdentifierToken(file, buffer, 0);
    else {
        buffer[0] = fgetc(file);
        *token = (Token){ .type = INVALID, .value = buffer };
    }
	
    return true;
}

Token* allTokens(FILE* file, int *length) {
    Token* tokens = (Token*)calloc(MAX_TOKEN_VECTOR_SIZE, sizeof(Token));
    Token token;
    int i = 0;
    while (readNextToken(file, &token)){	
    		if(((i>0 && strcmp(token.value,"\n")==0 && (strcmp(tokens[i-1].value,"\n")))!=0 || strcmp(token.value,"\n")!=0 ))
        		tokens[i++] = token;
    }
    *length = i;
    return tokens;
}
