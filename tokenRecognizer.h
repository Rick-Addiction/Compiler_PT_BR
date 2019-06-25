#ifndef TOKEN_RECOGNIZER_H
#define TOKEN_RECOGNIZER_H

#include <stdio.h>
#include "tokens.h"

/// Validates if the input is a number. Will keep reading from
/// file and inserting into buffer while it still matches a digit.
///
/// - parameter file: The file to be read
/// - parameter buffer: The current word being processed
/// - paramter i: The current buffer length
///
/// - returns: If sequence ends with a delimitator or whitespace, returns
/// a NUMBER type token, else returns an INVALID type token.
Token validateNumber(FILE* file, char* buffer, int i);

/// Validates if the input is a delimitator, will stop if a match is found immediately.
/// A delimitator should be comeposed of only one char
///
/// - parameter file: The file to be read
/// - parameter buffer: The current word being processed
/// - paramter i: The current buffer length
///
/// - returns: A DELIMITATOR type token if successful, INVALID otherwise
Token validateDelimitatorToken(FILE* file, char* buffer, int i);

/// Validates if the input is an operator
///
/// - parameter file: The file to be read
/// - parameter buffer: The current word being processed
/// - paramter i: The current buffer length
///
/// - returns: If sequence ends with a delimitator or whitespace, returns
/// a OPERATOR type token, INVALID otherwise
Token validateOperatorToken(FILE* file, char* buffer, int i);

/// Validates if the input is an identifier. will keep reading from
/// file and inserting into buffer while it still matches an identifier
/// char
///
/// - parameter file: The file to be read
/// - parameter buffer: The current word being processed
/// - paramter i: The current buffer length
///
/// - returns: An IDENTIFIER type token that stops at the first
/// non-identifier char read
Token validateIdentifierToken(FILE* file, char* buffer, int i);

/// Validates if the input is a reserved word. Will keep reading from
/// file and inserting into buffer while it still matches the chars from
/// a reserved word
///
/// - parameter file: The file to be read
/// - parameter buffer: The current word being processed
/// - paramter i: The current buffer length
///
/// - returns: A RESERVED_WORD type token if successful, attempts
/// to convert to an IDENTIFIER otherwise
Token validateReservedWordToken(FILE* file, char* buffer, int i);

/// After an invalid token is recognized, this function is called to keep
/// reading it in order to return the full invalid token.
/// 
/// - parameter file: The file to be read.
/// - parameter buffer: The current word being processed.
/// - paramter i: The current buffer length.
///
/// - returns: An INVALID type token.
Token getInvalidToken(FILE* file, char* buffer, int i);

#endif // TOKEN_RECOGNIZER_H
