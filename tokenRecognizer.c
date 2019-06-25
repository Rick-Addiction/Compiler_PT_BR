#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "tokens.h"
#include "tokenRecognizer.h"

Token validateNumber(FILE* file, char* buffer, int i) {
    char c;

    loop:
    c = fgetc(file);
    if (isDigit(c)) {
        buffer[i++] = c;
        goto loop;
    }

    if (isWhitespace(c) || isDelimitator(c) || c == EOF) {
        if (c != EOF)
            fseek(file, -1, SEEK_CUR);
        return (Token){ .type = NUMBER, .value = buffer };
    }

    fseek(file, -1, SEEK_CUR);
    return getInvalidToken(file, buffer, i);
}

Token validateDelimitatorToken(FILE* file, char* buffer, int i) {
    char c;

	c = fgetc(file);

    if (isDelimitator(c)) {  		
        buffer[i++] = c;
        return (Token){ .type = DELIMITATOR, .value = buffer };
    	}
    	
    
    
    fseek(file, -1, SEEK_CUR);
    return getInvalidToken(file, buffer, i);
}

Token validateOperatorToken(FILE* file, char* buffer, int i) {
    char c = fgetc(file);
    buffer[i++] = c;

    switch (c) {
        // when recognized, will immediately return a OPERATOR token
        case '*':
        case '+':
        case '-': goto arithmetic;
        case '=': goto relational;
        // :, :=
        case ':': goto delimitator;
        // >, >=
        case '>': goto q79;
        // <, <>, <=
        case '<': goto q81;
        default:
            return (Token){ .type = INVALID, .value = buffer };
    }

    // ----------------
    q79: // >, >=

    c = fgetc(file);
    switch (c) {
        case '=': // >=
            buffer[i++] = c;
            goto relational;

        default: // >
            fseek(file, -1, SEEK_CUR);
            goto relational;
    }

    // ----------------
    q81: // <, <=, <>

    c = fgetc(file);
    switch (c) {
    case '-': // <-
    		buffer[i++] = c;
            goto attribution;
    case '=': // <=
        buffer[i++] = c;
        goto relational;

    case '>': // <>
        buffer[i++] = c;
        goto relational;

    default: // <
        fseek(file, -1, SEEK_CUR);
        goto relational;
    }

    // ----------------
    delimitator:
    return (Token){ .type = DELIMITATOR, .value = buffer };

    // ----------------
    attribution: // +, -, *
    return (Token){ .type = ATTRIBUTION, .value = buffer };

    // ----------------
    arithmetic: // +, -, *
    return (Token){ .type = ARITHMETIC, .value = buffer };

    // ----------------
    relational: // =, :=, >, >=, <, <=, <>
    return (Token){ .type = RELATIONAL, .value = buffer };
}

/// Tries to match the next char stream from file with the 'expected' string.
/// Adds matches into buffer.
/// If it didn't match, the file pointer is returned to the last matched char
/// position then will be validated as identifier token type.
///
/// - parameter file: The file to be read
/// - parameter buffer: The buffer to be populated with file characters
/// - parameter i: Current buffer length
/// - parameter expected: The expected string to match
/// - parameter type: The expected token type
/// - parameter word: The expected token word
///
/// - returns: The token with matched value and type or an identifier token
Token validateMatchOrID(FILE* file, char* buffer, int i, char* expected, TokenType type, TokenWord word) {
    char c, k;
    int j = 0;

    loop:
    c = fgetc(file);
    k = expected[j++];

    if (k == '\0') {
        if (isWhitespace(c) || isDelimitator(c)
            || isInSet(c, operatorLetterSet) || c == EOF) {
            if (c != EOF)
                fseek(file, -1, SEEK_CUR);
            return (Token){ .type = type, .value = buffer, .word = word };
        }
        return validateIdentifierToken(file, buffer, i);
    }
    else if (c == k) {
        buffer[i++] = c;
        goto loop;
    }

    fseek(file, -1, SEEK_CUR);
    return validateIdentifierToken(file, buffer, i);
}

/// Continues to read the file sequence, trying to match the read sequence, with
/// the 'expected' string. Adds matches into buffer.
///
/// - parameter file: The file to be read
/// - parameter buffer: The buffer to be populated with file characters
/// - parameter i: Current buffer length pointer
/// - parameter c: Current char from file
/// - parameter expected: The expected string to match
///
/// - returns: If the next file sequence matched the expected string exactly
bool matchesSequence(FILE* file, char* buffer, int *i, char* expected) {
    char k, c;
    int j = 0;

    loop:
    c = fgetc(file);
    k = expected[j++];
    if (k == '\0') { // has matched
        // send file pointer back to the next char to be read
        fseek(file, -1, SEEK_CUR);
        return true;
    }

    if (c == k) { // still matching
        buffer[*i] = c;
        *i += 1;
        goto loop;
    }

    // send file pointer back to the next char to be read
    fseek(file, -1, SEEK_CUR);
    return false;
}

Token validateIdentifierToken(FILE* file, char* buffer, int i) {
    char c;
    loop:
    c = fgetc(file);

    if (isValidIdentifier(c)) {
        buffer[i++] = c;
        goto loop;
    }

    if(isDelimitator(c) || isWhitespace(c)
        || isInSet(c, operatorLetterSet) || c == EOF) {
        if (c != EOF)
            fseek(file, -1, SEEK_CUR);
        return (Token){ .type = IDENTIFIER, .value = buffer };
    }

    if (c != EOF)
        fseek(file, -1, SEEK_CUR);

    return getInvalidToken(file, buffer, i);
}

Token getInvalidToken(FILE* file, char* buffer, int i) {
    char c;

    loop:
    c = fgetc(file);
    if(isDelimitator(c) || c == EOF || isWhitespace(c)) {
        if (c != EOF)
            fseek(file, -1, SEEK_CUR);
        return (Token){ .type = INVALID, .value = buffer };
    }

    buffer[i++] = c;
    goto loop;
}

Token validateReservedWordToken(FILE* file, char* buffer, int i) {
    switch (fpeekc(file)) {
        case 'a':
        	switch (fpeek2c(file,1)) {
        		case 'l':
        		return validateMatchOrID(file, buffer, i, "algoritmo", RESERVED_WORD, W_PROGRAM);   
				case 't':
				return validateMatchOrID(file, buffer, i, "ate", RESERVED_WORD, W_TO);        
        	 default:
                    goto validate_as_ID;
        	}
        case 'd': // do, div
            switch (fpeek2c(file,1)) {
                case 'i':
                    return validateMatchOrID(file, buffer, i, "divisao", ARITHMETIC, W_DIV);
            	case 'e':
            		return validateMatchOrID(file, buffer, i, "de", RESERVED_WORD, W_FROM);
                default:
                    goto validate_as_ID;
            }
        case 'e': // else, end
            switch (fpeek2c(file,1)) {
                case 'n':
                		switch(fpeek2c(file,2)){
							case 't':
            				return validateMatchOrID(file, buffer, i, "entao", RESERVED_WORD, W_THEN);
            				case 'q':
            					    return validateMatchOrID(file, buffer, i, "enquanto", RESERVED_WORD, W_WHILE);
            					}
            	case 's':
            		return validateMatchOrID(file, buffer, i, "escreva", RESERVED_WORD, W_WRITE);
                default:
                	return validateMatchOrID(file, buffer, i, "e", LOGIC, W_AND);
				
            }
        case 'f': // false
        	switch(fpeek2c(file,1)){
			case 'a':
				switch(fpeek2c(file,2)){
					case 'c':
						case 'a':
                    return validateMatchOrID(file, buffer, i, "faca", RESERVED_WORD, W_DO);
                    case 'l':
					 return validateMatchOrID(file, buffer, i, "falso", BOOLEAN, W_FALSE);
        	}
        	case 'i':
        		switch(fpeek2c(file,2)){
        			case 'm':
        				switch(fpeek2c(file,3)){
        					case 'a':
        						return validateMatchOrID(file, buffer, i, "fimalgoritmo", RESERVED_WORD, W_END);	
        					case 's':
        						return validateMatchOrID(file, buffer, i, "fimse", RESERVED_WORD, W_ENDIF);
        					case 'e':
								return validateMatchOrID(file, buffer, i, "fimenquanto", RESERVED_WORD, W_ENDWHILE);
							case 'p':
								return validateMatchOrID(file, buffer, i, "fimpara", RESERVED_WORD, W_ENDFOR);
        				}
        		}
        	default:
                    goto validate_as_ID;
        	}
        case 'i': // if, integer
            switch (fpeek2c(file,1)) {
                case 'n':
                		switch(fpeek2c(file,2)){
						case 'i':
							return validateMatchOrID(file, buffer, i, "inicio", RESERVED_WORD, W_BEGIN);
						case 't':	
                    		return validateMatchOrID(file, buffer, i, "inteiro", TYPE, W_INTEGER);
                    	default:
                    		goto validate_as_ID;
						}
                default:
                    goto validate_as_ID;
            }
        case 'l':
        	switch(fpeek2c(file,1)){
        		case 'e':
					return validateMatchOrID(file, buffer, i, "leia", RESERVED_WORD, W_READ);   
				case 'o':
			    	return validateMatchOrID(file, buffer, i, "logico", TYPE, W_BOOLEAN);
				default:
                    goto validate_as_ID;    
		}
        case 'n': // not
            return validateMatchOrID(file, buffer, i, "nao", LOGIC, W_NOT);
        case 'o': // or
            return validateMatchOrID(file, buffer, i, "ou", LOGIC, W_OR);
        case 'p':
        	switch(fpeek2c(file,1)){
        		case 'a':
        			switch(fpeek2c(file,2)){
        				case 'r':
        					return validateMatchOrID(file, buffer, i, "para", RESERVED_WORD, W_FOR);
        				case 's':
        					return validateMatchOrID(file, buffer, i, "passo", RESERVED_WORD, W_INCREMENT);
        			default:
                    goto validate_as_ID;
        			}
        	}
        case 's':
			switch(fpeek2c(file,1)){
				case 'e':
					switch(fpeek2c(file,2)){
						case 'n':
							return validateMatchOrID(file, buffer, i, "senao", RESERVED_WORD, W_ELSE);
						default:
							return validateMatchOrID(file, buffer, i, "se", RESERVED_WORD, W_IF);
						}
				default:
                    goto validate_as_ID;
                }
			    
        
        case 'v':
			switch(fpeek2c(file,1)){
				case 'a':
			        return validateMatchOrID(file, buffer, i, "var", RESERVED_WORD, W_VAR);
			    case 'e':
					return validateMatchOrID(file, buffer, i, "verdadeiro", BOOLEAN, W_TRUE);    
			}
        
        default:
            goto validate_as_ID;
    }

    validate_as_ID:
    return validateIdentifierToken(file, buffer, i);
}
