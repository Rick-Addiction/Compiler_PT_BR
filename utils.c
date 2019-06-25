#include <stdbool.h>
#include <stdio.h>
#include "utils.h"

CharSet reservedLetterSet = { .values = "abdefilnopstvw" };
CharSet unreservedLetterSet = { .values = "cghjkmqruyxz" };
CharSet operatorLetterSet = { .values = "-+*<>:=" };

bool isDigit(char c) {
    return (c >= 48 && c <= 57);
}

bool isLetter(char c) {
    return (c >= 65 && c <= 90) // A-Z
        || (c >= 97 && c <= 122); // a-z
}

bool isAlphanumeric(char c) {
    return isDigit(c) || isLetter(c);
}

bool isOperator(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/': return true;
        default: return false;
    }
}

bool isDelimitator(char c) {
    switch (c) {
    	case '\n':
    	case ' ':
        case '(':
        case ')':
        case ',':
        case ':':
        case '\"':
        case '.': return true;
        default: return false;
    }
}

bool isValidIdentifier(char c) {
    return isAlphanumeric(c) || (c == '_');
}

bool isWhitespace(char c) {
    switch (c) {
        case ' ': return true;
        case '\0': return true;
        default: return false;
    }
}

bool isInSet(char c, CharSet set) {
    char s;
    int i = 0;
    while((s = set.values[i++]) != '\0')
        if (s == c)
            return true;
    return false;
}

char fpeekc(FILE* file) {
    char c = fgetc(file);
    // when file pointer encounters EOF it doesn't move
    // this is to avoid going back one extra char
    if (c != EOF)
        fseek(file, -1, SEEK_CUR);
    return c;
}

char fpeek2c(FILE* file, int numWords) {
    if (fpeekc(file) == EOF)
        return EOF;
    fseek(file, numWords, SEEK_CUR);
    char c = fpeekc(file);
    fseek(file, numWords*-1, SEEK_CUR);
    return c;
}


