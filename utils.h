#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_TOKEN_VECTOR_SIZE 2000

typedef struct CharSet {
    char *values;
} CharSet;

CharSet reservedLetterSet;
CharSet unreservedLetterSet;
CharSet operatorLetterSet;

/// is whether a character is a digit [0-9]
///
/// -parameter c: The character to validate
///
/// - returns: Boolean value indicating if the character matches [0-9]
bool isDigit(char c);

/// Checks whether a character is an alphabet letter [a-zA-Z]
///
/// -parameter c: The character to validate
///
/// - returns: Boolean value indicating if the character matches [a-zA-Z]
bool isLetter(char c);

/// Checks whether a character is an alphanumeric character [a-zA-Z0-9]
///
/// -parameter c: The character to validate
///
/// - returns: Boolean value indicating if the character matches [a-zA-Z0-9]
bool isAlphanumeric(char c);

/// Checks whether a character is an operator [+*-/]
///
/// -parameter c: The character to validate
///
/// - returns: Boolean value indicating if the character matches [0-9]
bool isOperator(char c);

/// Checks whether a character is a delimitator [(),.:; ]
///
/// - parameter c: The character to validate
///
/// - returns: Boolean value indicating if the character matches [(),.:; ]
bool isDelimitator(char c);

/// Checks whether a character is a valid identifier [a-zA-Z0-9_]
///
/// - parameter c: The character to validate
///
/// - returns: Boolean value indicating if the character matches [a-zA-Z0-9_]
bool isValidIdentifier(char c);

/// Checks whether a character is a whitespace [ \n\0]
///
/// - parameter c: The character to validate
///
/// - returns: Boolean value indicating if the char matches [ \n\0]
bool isWhitespace(char c);

/// Checks whether a character is within a CharSet
///
/// - parameter c: The character to validate
///
/// - returns: Boolean value indicating if the char was found in CharSet
bool isInSet(char c, CharSet set);

/// Gets the next char without changing file pointer position
///
/// - parameter file: The file to be read
///
/// - returns: The next char
char fpeekc(FILE* file);

/// Gets the second next char without changing file pointer position
///
/// - parameter file: The file to be read
///
/// - returns: The second next char
char fpeek2c(FILE* file, int numWords);

#endif //UTILS_H
