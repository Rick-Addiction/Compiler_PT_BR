#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <string.h>
#include "utils.h"
#include "tokenizer.h"
#include "sintax.h"

/* -------------------------
    Declarations
------------------------- */
#define DEBUG 0

/* -------------------------
    Function Implementations
------------------------- */
int printTokens(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Nao pode abrir o arquivo (%s)\n", filename);
        return 1;
    }

    int length;
    Token* tokens = allTokens(file, &length);
    //for (int i = 0; i < length; ++i)
    //    printf("%s\n", tokenDescription(tokens[i]));

    if (isValidProgram(tokens, length)){
        printf("\n\nPrograma Valido\n");
    } else {
        printf("\n\nPrograma Invalido\n");
    }

    fclose(file);

    return 0;
}

/* -------------------------
    Function Bodies
------------------------- */
int main() {

    return printTokens("program.txt");
    printf("\n\nPress ENTER key to Continue\n");  
	getchar(); 
}
