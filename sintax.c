#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "sintax.h"

#include "uthash/uthash.h"

struct Node *head;
int symbolsTableQuantity = 0;
char* procedures[100];
Token* list;

bool stx_exprSimple(Token* tokens, int length, int* pos);
bool stx_command(Token* tokens, int length, int* pos);
bool stx_compositeCommand(Token* tokens, int length, int* pos);
bool stx_compositeCommand2(Token* tokens, int length, int* pos, char commandType[]);
bool stx_block(Token* tokens, int length, int* pos);
void call_exception(Token* tokens, int errorCode, int length, int* pos);
bool flagExp = 1;

bool addToCurrentSymbolsTable(Token *t, int length, int pos);
void addNewSymbolsTable();
void removeSymbolsTable();
Symbols* getCurrentSymbolsTable();
TokenWord getType(Token* tokens, int length, int pos);
Symbols* getGlobalVariablesTable();
bool variableMatchExpressionType(Token identifier, int syntaxTokenPosition);

/// Adds a new hash table at the beggining of the list.
void addNewSymbolsTable() {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->symbols = NULL;
    newNode->next = head;

    symbolsTableQuantity++;

    head = newNode;
}

/// Removes the current (HEAD) hash table from list.
void removeSymbolsTable() {
    symbolsTableQuantity--;
    head = head->next;
}

/// - returns: Pointer to the hash table.
Symbols* getCurrentSymbolsTable() {
    return head->symbols;
}

/// -parameter value: Char value.
bool variableExistsAtCurrentTable(char* value) {
    struct Symbols *s = NULL;

    HASH_FIND_STR(head->symbols, value, s);

    return s != NULL;
}

bool variableExistsAtGlobalEscope(char* value) {
    struct Symbols *s = NULL;

    Node *n = head;
    while (n->next) {
        n = n->next;
    }

    HASH_FIND_STR(n->symbols, value, s); 

    return s != NULL;
}

/// -parameter t: Token pointer.
bool addToCurrentSymbolsTable(Token *t, int length, int pos) {
    if (variableExistsAtCurrentTable(t->value)) {
        call_exception(list, 18, length, &pos);
        return false;
    }

    Symbols* s = (Symbols*) malloc(sizeof(Symbols));
    s->varName = t->value;
    s->varType = getType(t, length, pos);

    HASH_ADD_KEYPTR(hh, head->symbols, s->varName, strlen(s->varName), s);

    return true;
}

/// Prints all the values at all tables 
void printTables() {

    int count = symbolsTableQuantity;
    Node *n = head;
    while (n != NULL) {
        printf("Tabela do programa '%s':\n", procedures[count]);
        struct Symbols *s = n->symbols;
        for(; s != NULL; s=s->hh.next) {
            printf("Chave: '%s' / Tipo: '%s'\n", s->varName, tokenWordDescription(s->varType));
        }
        count--;
        n = n->next;
    }
}

Symbols* getGlobalVariablesTable() {
    Node *n = head;
    while (n != NULL) {
        n = n->next;
    }

    return n->symbols;
}


TokenWord getType(Token* tokens, int length, int pos);

/// Receives the analyzed token and looks for the next token which is of type 'TYPE' 
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: String value for the type.
TokenWord getType(Token* tokens, int length, int pos) {
    Token token;
    while (pos < length) {
        token = list[pos];

        if (token.type == TYPE) {
            return token.word;
        }

        pos++;
    }

    return UNKOWN_WORD;
}

/// Receives the identifier Token, and the position on the list of Tokens where the syntax expression begins.
/// This method checks to see if the identifier exists in the symbols table in the current context and at the global variables.
/// If so, it checks if the type of the identifier matches the syntax expression.
///
/// -parameter identifier: Token of the identifier.
/// -parameter syntaxTokenPosition: Position on the list where the syntax expression begins.
///
/// - returns: Boolean value indicating if it matches.
bool variableMatchExpressionType(Token identifier, int syntaxTokenPosition) {
    Token token = list[syntaxTokenPosition];
    // printf("%s", tokenDescription(token));

    if (token.type == DELIMITATOR) {
        return true;
    } else if (token.type != IDENTIFIER && token.type != NUMBER && token.type != BOOLEAN) {
        return variableMatchExpressionType(identifier, (syntaxTokenPosition+1));
    }

    struct Symbols *s = NULL;
    Node *n = head;
    while (n->next) {
        n = n->next;
    }

    HASH_FIND_STR(head->symbols, identifier.value, s); 

    if (s == NULL) {
        HASH_FIND_STR(n->symbols, identifier.value, s); 
    }

    if (s == NULL) {
        call_exception(list, 19, 0, &syntaxTokenPosition);
        return false;
    }

    Token t = token;

        

    if (t.type == IDENTIFIER) {
        struct Symbols *v = NULL;
        bool match = false;
        // printf("aaaa");
        if (variableExistsAtCurrentTable(t.value)) {
            // printf("ddddd");
            HASH_FIND_STR(head->symbols, t.value, v);
            // printf("Var v %s", tokenWordDescription(v->varType));
            // printf("Var s %s", tokenWordDescription(s->varType));
            match = v->varType == s->varType;
        } else if (variableExistsAtGlobalEscope(t.value)) {
            // printf("xxxxx");
            HASH_FIND_STR(n->symbols, t.value, v); 
            // printf("Var v %s", tokenWordDescription(v->varType));
            // printf("Var s %s", tokenWordDescription(s->varType));
            match = v->varType == s->varType;
        } 

        if (!match) {
            call_exception(list, 17, 0, &syntaxTokenPosition);
            return false;
        }

        return variableMatchExpressionType(identifier, (syntaxTokenPosition+1));
    }

    bool typeMatch = (s->varType == W_INTEGER && t.type == NUMBER) || (s->varType == W_BOOLEAN && t.type == BOOLEAN);

    if (!typeMatch) {
        call_exception(list, 17, 0, &syntaxTokenPosition);
        return false;
    }

    return variableMatchExpressionType(identifier, (syntaxTokenPosition+1));
}


/// Prints an exception message. 
///
/// -parameter tokens: Token array pointer.
/// -parameter errorCode: Code that corresponds to a error message.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
void call_exception(Token* tokens, int errorCode, int length, int* pos){
    Token token = tokens[*pos];
    if(flagExp){
        flagExp = 0;
        switch (errorCode){

        case 0: //program reserved word not found
        printf("\nEXPECTED algoritmo RECEBIDO: %s" "\n", token.value);
        break;

        case 1: //identifier error
        printf("\ESPERADO identificador, RECEBIDO: %s" "\n", token.value);
        break;

        case 2:
        printf("\ESPERADO quebra de linha, RECEBIDO: %s" "\n", token.value);
        break;

        case 3:
        printf("\ESPERADO . , RECEBIDO: %s" "\n", token.value);
        break;

        case 4:
        printf("\ESPERADO identificador, RECEBIDO: %s" "\n", token.value);
        break;

        case 5:
        printf("\ESPERADO :, RECEBIDO: %s" "\n", token.value);
        break;

        case 6:
        printf("\ESPERADO inteiro u booleano, RECEBIDO: %s" "\n", token.value);
        break;

        case 7:
        printf("\ESPERADO identificador, identificador, RECEBIDO: %s" "\n", token.value);
        break;

        case 8:
        printf("\nEXPECTED A ¨procedure¨ RECEIVED: %s" "\n", token.value);
        break;

        case 9:
        printf("\nINCORRECT DECLARATION OF A PROCEDURE\n");
        break;

        case 10:
        printf("\nPROCEDURE PARAMETER ERROR \n");
        break;

        case 11:
        printf("\nDECLARAO DE TERMO ERRADA, RECEBIDO: %s \n", token.value);
        break;

        case 12:
        printf("\nOPERACAO ARITMETICA COM TERMO ERRADO, RECEBIDO: %s" "\n", token.value);
        break;

        case 13:
        printf("\nCOMANDO DE ESCREVA ERRADO\n");
        break;

        case 14:
        printf("\nDECLARACAO DE EXPRESSAO ERRADA \n");
        break;

        case 15:
        printf("\Esperado um INICIO <EXPRESSAO> FIM \n");
        break;

        case 16:
        printf("\nErro inesperado no comando: '%s' \n", token.value);
        break;

        case 17:
            printf("\nTipo errado atribuido a variavel '%s' \n", token.value);
            break;		
	     
        case 18:
            printf("\nVariavel ja existe na tabela de simbolos: %s \n", list[*pos-1].value);
            break;

        case 19:
            printf("\nVariavael nao existe: %s \n", list[*pos-2].value);
            break;
        }
      
    }

    exit(EXIT_FAILURE);
}


/* -------------------------
    TOKEN VALIDATIONS
------------------------- */
/// Verifies if a token at a given index is equal to the passed type. 
/// Increases the position index by one if so.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
/// -parameter type: The expected token type.
///
/// - returns: Boolean value indicating if the token types match.
bool isTypeOf(Token* tokens, int length, int* pos, TokenType type) {
    Token token = tokens[*pos];
    if (token.type != type)
        return false;
    (*pos)++;
    return true;
}

bool isTypeOf2(Token* tokens, int length, int* pos, TokenType type) {
    Token token = tokens[*pos];
    if (token.type != type || !variableExistsAtCurrentTable(tokens[*pos].value))
    	
        return false;
    (*pos)++;
    return true;
}

/// Verifies if a token at a given index is delimited by the passed string.
/// Increases the position index by one if so.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
/// -parameter value: The string that is expected to delimit it.
///
/// - returns: Boolean value indicating if the token value matches the passed value.
bool isDelimitatorOf(Token* tokens, int length, int* pos, char* value) {
    Token token = tokens[*pos];
    if (!(token.type == DELIMITATOR
        && !strcmp(token.value, value)))
        return false;
    (*pos)++;
    return true;
}

/// Verifies if a token at a given index is of arithmetic type and matches the passed value.
/// Increases the position index by one if so.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
/// -parameter value: Expected arithmetic value.
///
/// - returns: Boolean value indicating if the token value matches the passed value.
bool isArithmetic(Token* tokens, int length, int* pos, char* value) {
    Token token = tokens[*pos];
    if (!(token.type == ARITHMETIC
        && !strcmp(token.value, value)))
        return false;
    (*pos)++;
    return true;
}

/// Verifies if a token at a given index is the expected reserverd word type.
/// Increases the position index by one if so.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
/// -parameter word: Expected reserved word.
///
/// - returns: Boolean value indicating if the token word matches the passed one.
bool isReservedWordTypeOf(Token* tokens, int length, int* pos, TokenWord word) {
    Token token = tokens[*pos];
    if (!(token.type == RESERVED_WORD
        && token.word == word))
        return false;
    (*pos)++;
    return true;
}

/// Verifies if a token at a given index is a logic operator.
/// Increases the position index by one if so.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if the token word is either W_AND or W_OR.
bool isLogicOperator(Token* tokens, int length, int* pos) {
    Token token = tokens[*pos];
    if (!(token.type == LOGIC
        && (token.word == W_AND || token.word == W_OR)))
        return false;
    (*pos)++;
    return true;
}

/// Verifies if a token at a given index is a boolean.
/// Increases the position index by one if so.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if the token word is either W_TRUE or W_FALSE.
bool isBoolean(Token* tokens, int length, int* pos) {
    Token token = tokens[*pos];
    if (!(token.type == BOOLEAN
        && (token.word == W_TRUE || token.word == W_FALSE)))
        return false;
    (*pos)++;
    return true;
}

/* -------------------------
    SYNTAX VALIDATIONS
------------------------- */
/// Verifies if the syntax of a list of identifiers is correct.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_identifierList(Token* tokens, int length, int* pos) {
    bool isType = 0;

    isType = isTypeOf(tokens, length, pos, IDENTIFIER);

    if (!isType){
        call_exception (tokens, 1, length, pos);
        return false;
    }

	bool addVariable = addToCurrentSymbolsTable(&(tokens[(*pos)-1]), length, *pos);
	
	if (!addVariable) {
        return false;
    }

    loop:
    if (isDelimitatorOf(tokens, length, pos, ",")) {
        if (isTypeOf(tokens, length, pos, IDENTIFIER)) {
			bool addVariable = addToCurrentSymbolsTable(&(tokens[(*pos)-1]), length, *pos);

            if (!addVariable) {
                return false;
            }

            goto loop;
        }
        else{
            call_exception (tokens, 7, length, pos);
            return false;
        }
    }
    return true;
}

/// Verifies if the syntax of a variable declaration is correct.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_varDeclaration(Token* tokens, int length, int* pos) {
    bool _identifier = 0, isDelimitatorTwoDots = 0, isType = 0;

    _identifier = stx_identifierList(tokens, length, pos);

    if(_identifier){
        isDelimitatorTwoDots = isDelimitatorOf(tokens, length, pos, ":");
    }

    if (_identifier && !isDelimitatorTwoDots){
        call_exception (tokens, 5, length, pos);
    }

    if(_identifier && isDelimitatorTwoDots){
        isType = isTypeOf(tokens, length, pos, TYPE);
    }

    if(isDelimitatorTwoDots && !isType && _identifier){
        call_exception (tokens, 6, length, pos);
    }

    return _identifier
        && isDelimitatorTwoDots
        && isType;
}

/// Verifies if the syntax for the part where the variables are declared is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_varPartDeclaration(Token* tokens, int length, int* pos) {
    bool isVar = 0, _varDeclaration = 0, isDelimitatorLine = 0;

    isVar = isReservedWordTypeOf(tokens, length, pos, W_VAR);

    if (!isVar){
        call_exception(tokens, 4, length, pos);
        return false;
    }


    loop:

    _varDeclaration = stx_varDeclaration(tokens, length, pos);
    isDelimitatorLine = isDelimitatorOf(tokens, length, pos, "\n");

    if (_varDeclaration && !isDelimitatorLine){
        call_exception(tokens, 2, length, pos);
        return false;
    }

    isVar = isReservedWordTypeOf(tokens, length, pos, W_VAR);

    if (isVar && _varDeclaration && isDelimitatorLine){
        goto loop;
    }

    return true;
}

/// Verifies if the syntax for the parameters list in a procedure declaration is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_formalParameters(Token* tokens, int length, int* pos) {
    if (!isDelimitatorOf(tokens, length, pos, "(")){
        call_exception(tokens, 10, length, pos);
        return false;
    }

    loop:
    if (!(isReservedWordTypeOf(tokens, length, pos, W_VAR)
        && stx_identifierList(tokens, length, pos)
        && isDelimitatorOf(tokens, length, pos, ":")
        && isTypeOf(tokens, length, pos, TYPE))) {

        call_exception(tokens, 9, length, pos);
        return false;
    }

    if (isDelimitatorOf(tokens, length, pos, "\n"))
        goto loop;

    return isDelimitatorOf(tokens, length, pos, ")");
}

/// Verifies if the syntax of a procedure declaration is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_procedureDeclare(Token* tokens, int length, int* pos) {

    bool isReserved = 0, isType = 0, _formaParam = 0;
    isReserved = isReservedWordTypeOf(tokens, length, pos, W_PROCEDURE);

    isType = isTypeOf(tokens, length, pos, IDENTIFIER);

    if(isReserved && !isType){
        call_exception(tokens, 1, length, pos);
    }


    if(isReserved && isType){
        _formaParam = stx_formalParameters(tokens, length, pos);
    }

    return isReserved
        && isType
        && _formaParam
        && isDelimitatorOf(tokens, length, pos, "\n")
        && stx_block(tokens, length, pos)
        && isDelimitatorOf(tokens, length, pos, "\n");
}



/// Verifies if the syntax for a condition is valid.
/// e.g.: booleanVariable, true, (expression)
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_factor(Token* tokens, int length, int* pos) {
    return isTypeOf2(tokens, length, pos, IDENTIFIER)
        || isTypeOf(tokens, length, pos, NUMBER)
        || isBoolean(tokens, length, pos)
        || (isDelimitatorOf(tokens, length, pos, "(")
            && stx_exprSimple(tokens, length, pos)
            && isDelimitatorOf(tokens, length, pos, ")"));
}

/// Verifies if the syntax for a term is valid.
/// e.g.: fator * fator
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_term(Token* tokens, int length, int* pos) {
    if (!stx_factor(tokens, length, pos))
        return false;

    loop:
    if (isArithmetic(tokens, length, pos, "*")
        || isArithmetic(tokens, length, pos, "div")) {
        if (stx_factor(tokens, length, pos))
            goto loop;
        return false;
    }
    return true;
}

/// Verifies if the syntax for a simple expression is valid.
/// e.g.: term + term
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_exprSimple(Token* tokens, int length, int* pos) {
    // optional
    isArithmetic(tokens, length, pos, "-");

    if (!stx_term(tokens, length, pos)){
        call_exception(tokens, 11, length, pos);
        return false;
    }

    loop:
    if (isArithmetic(tokens, length, pos, "+")
        || isArithmetic(tokens, length, pos, "-")) {
        if (stx_term(tokens, length, pos)){
            goto loop;
        }

        else{
            call_exception(tokens, 12, length, pos);
            return false;
        }
    }
    return true;
}

/// Verifies if the syntax for an extended expression is valid.
/// e.g.: simpleExpression (RELATIONAL TOKEN) simpleExpression
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_exprExt(Token* tokens, int length, int* pos) {
    Token t1 = tokens[*pos];
	TokenWord t1w;
	if (!stx_exprSimple(tokens, length, pos)) {
        return false;
    }
    
    struct Symbols *s = NULL;
    if(t1.type==IDENTIFIER){
	HASH_FIND_STR(head->symbols, t1.value, s);
	t1w = s->varType;
	}
	else if(t1.type==NUMBER){	
	t1w = W_INTEGER;	
	}
	else{
		t1w = W_BOOLEAN;	
	}

	
    loop:
    if (isTypeOf(tokens, length, pos, RELATIONAL)) {
    	bool match = false;
		Token t2 = tokens[*pos]; 
		TokenWord t2w;   	
    	struct Symbols *v = NULL; 
        if(t2.type==IDENTIFIER){
		HASH_FIND_STR(head->symbols, t2.value, v);
		t2w = s->varType;
		}
		else if(t2.type==NUMBER){
		t2w = W_INTEGER;	
		}
		else{
		t2w = W_BOOLEAN;	
		}
		
    	if(t1w==t2w)
	       	if (stx_exprSimple(tokens, length, pos))
	            return true;

        return false;
    }

    return true;
}

/// Verifies if the syntax for an expression is valid.
/// e.g.: a OR b
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_expr(Token* tokens, int length, int* pos) {
    int a = stx_exprExt(tokens, length, pos);
    // printf("|-expr exprExt: %d\n", a);
    if (!a) {
        call_exception(tokens, 14, length, pos);
        return false;
    }

    loop:
    if (isLogicOperator(tokens, length, pos)) {
        if (stx_expr(tokens, length, pos)){
            goto loop;

        }
        call_exception(tokens, 14, length, pos);
        return false;
    }
    return true;
}

/// Verifies if the syntax for a variable attribution is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_attribution(Token* tokens, int length, int* pos) {
    Token identifier = tokens[*pos];

    bool isIdentifier = isTypeOf(tokens, length, pos, IDENTIFIER);
    bool isAttribution = isTypeOf(tokens, length, pos, ATTRIBUTION);

    int position = *pos;

    bool isValidSyntax = isIdentifier && isAttribution && stx_expr(tokens, length, pos);

    if (!isValidSyntax) {
        return false;
    }

     return variableMatchExpressionType(identifier, position);
}

/// Verifies if the syntax for a parameter list in a procedure is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_parameterList(Token* tokens, int length, int* pos) {
    if (!(isTypeOf(tokens, length, pos, IDENTIFIER)
        || isTypeOf(tokens, length, pos, NUMBER)
        || isBoolean(tokens, length, pos)))
        return false;

    loop:
    if (isDelimitatorOf(tokens, length, pos, ",")) {
        if ((isTypeOf(tokens, length, pos, IDENTIFIER)
            || isTypeOf(tokens, length, pos, NUMBER)
            || isBoolean(tokens, length, pos)))
            goto loop;
        else
            return false;
    }
    return true;
}

/// Verifies if the syntax for calling a procedure is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_procedureCall(Token* tokens, int length, int* pos) {
    if (!(isTypeOf(tokens, length, pos, IDENTIFIER)
        && isDelimitatorOf(tokens, length, pos, "(")))
        return false;
    return  isDelimitatorOf(tokens, length, pos, ")")
        || (stx_parameterList(tokens, length, pos)
            && isDelimitatorOf(tokens, length, pos, ")"));
}

/// Verifies if the syntax for a conditional expression is valid.
/// e.g.: if (expression) then command
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_conditional(Token* tokens, int length, int* pos) {
	int inicial = *pos;
    if (!(isReservedWordTypeOf(tokens, length, pos, W_IF)
        && stx_expr(tokens, length, pos)
        && isReservedWordTypeOf(tokens, length, pos, W_THEN)
        && stx_compositeCommand2(tokens, length, pos,"W_IF")
		&& isDelimitatorOf(tokens, length, pos, "\n"))){
		*pos = inicial;
		return false;
	}
    //(*pos)++; // lookahead
    (*pos)=(*pos)-2;
	if (isReservedWordTypeOf(tokens, length, pos, W_ELSE))
        return stx_compositeCommand2(tokens, length, pos,"W_IF");
    
    if (!(isReservedWordTypeOf(tokens, length, pos, W_ENDIF))){
		call_exception (tokens, 18, length, pos);
		return false;
	}

    return true;
}

/// Verifies if the syntax for a while expression is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_while(Token* tokens, int length, int* pos) {
   if (!(isReservedWordTypeOf(tokens, length, pos, W_WHILE)
        && stx_expr(tokens, length, pos)
        && isReservedWordTypeOf(tokens, length, pos, W_DO)
        && stx_compositeCommand2(tokens, length, pos,"W_WHILE")
		&& isDelimitatorOf(tokens, length, pos, "\n")))
		return false;
	(*pos)=(*pos)-2;	
	if (!(isReservedWordTypeOf(tokens, length, pos, W_ENDWHILE))){
		call_exception (tokens, 18, length, pos);
		return false;
	}
	
	return true;
}

bool stx_for(Token* tokens, int length, int* pos) {
   if (!(isReservedWordTypeOf(tokens, length, pos, W_FOR)
   		&& isTypeOf(tokens, length, pos, IDENTIFIER)
   		&& isReservedWordTypeOf(tokens, length, pos, W_FROM)
   		&& isTypeOf(tokens, length, pos, NUMBER)
        && isReservedWordTypeOf(tokens, length, pos, W_TO)
        && isTypeOf(tokens, length, pos, NUMBER)
        && isReservedWordTypeOf(tokens, length, pos, W_INCREMENT)
        && isTypeOf(tokens, length, pos, NUMBER)
        && isReservedWordTypeOf(tokens, length, pos, W_DO)
        && stx_compositeCommand2(tokens, length, pos,"W_FOR")
		&& isDelimitatorOf(tokens, length, pos, "\n")))
		return false;
	(*pos)=(*pos)-2;	
	if (!(isReservedWordTypeOf(tokens, length, pos, W_ENDFOR))){
		call_exception (tokens, 18, length, pos);
		return false;
	}
	
	return true;	
}

/// Verifies if the syntax for calling the write method is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_writeText(Token* tokens, int length, int* pos) {
    

    if (isTypeOf2(tokens, length, pos, IDENTIFIER)){
	
		if(isDelimitatorOf(tokens, length, pos, ","))
			goto loop;
		else if(isDelimitatorOf(tokens, length, pos, ")"))
    		return true;
		else{
    		call_exception(tokens, 13, length, pos);
        	return false;
			}		        	
    }else if(isDelimitatorOf(tokens, length, pos, "\"")){
    	while(!(isDelimitatorOf(tokens, length, pos, "\"")
				|| isDelimitatorOf(tokens, length, pos, "\n")
			   )
			  && (*pos) < length){
			  (*pos)++; 	
			  }
			(*pos)--;  
			if(isDelimitatorOf(tokens, length, pos, "\""))
					if(isDelimitatorOf(tokens, length, pos, ","))
						goto loop;
					else if(isDelimitatorOf(tokens, length, pos, ")"))
    					return true;
    				else{
    					call_exception(tokens, 13, length, pos);
        				return false;
					}
    		else{
    			call_exception(tokens, 13, length, pos);
        		return false;
			}
    			
    		
    }
    else{
    	call_exception(tokens, 13, length, pos);
        return false;    	
	}


    
	loop:

    if (isTypeOf2(tokens, length, pos, IDENTIFIER)){
	
		if(isDelimitatorOf(tokens, length, pos, ","))
			goto loop;
		else if(isDelimitatorOf(tokens, length, pos, ")"))
    		return true;
		else{
    		call_exception(tokens, 13, length, pos);
        	return false;   
		}
    }else if(isDelimitatorOf(tokens, length, pos, "\"")){
    	while(!(isDelimitatorOf(tokens, length, pos, "\"")
				|| isDelimitatorOf(tokens, length, pos, "\n")
			   )
			  && (*pos) < length){
			  (*pos)++; 	
			  }
			(*pos)--;   
			if(isDelimitatorOf(tokens, length, pos, "\""))
    			if(isDelimitatorOf(tokens, length, pos, ","))
						goto loop;
					else if(isDelimitatorOf(tokens, length, pos, ")"))
    					return true;
    				else{
    					call_exception(tokens, 13, length, pos);
        				return false;
					}
    		else{
    			call_exception(tokens, 13, length, pos);
        		return false;
			}
    			
    		
    }
    else if(!(isDelimitatorOf(tokens, length, pos, ")"))){
    	call_exception(tokens, 13, length, pos);
        return false;    	
	}	
	

    return true;
}


bool stx_write(Token* tokens, int length, int* pos) {
    bool writeDeclare = 0, commandWrite = 0;

    writeDeclare = isReservedWordTypeOf(tokens, length, pos, W_WRITE);

    if(writeDeclare){
        commandWrite =
            (isDelimitatorOf(tokens, length, pos, "(")
            && stx_writeText(tokens, length, pos));
    }

    if(writeDeclare && !commandWrite){
        call_exception (tokens, 13, length, pos);
    }

    return commandWrite;
}

bool stx_read(Token* tokens, int length, int* pos) {
    bool readDeclare = 0, commandRead = 0;

    readDeclare = isReservedWordTypeOf(tokens, length, pos, W_READ);

    if(readDeclare){
        commandRead =
            (isDelimitatorOf(tokens, length, pos, "(")
            && isTypeOf2(tokens, length, pos, IDENTIFIER)
            && isDelimitatorOf(tokens, length, pos, ")"));
    }

    if(readDeclare && !commandRead){
        call_exception (tokens, 13, length, pos);
    }

    return commandRead;
}

/// Verifies if the syntax for a command is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_command(Token* tokens, int length, int* pos) {
    int current = *pos;
    if (stx_attribution(tokens, length, pos))
        return true;

    *pos = current;
    if (stx_procedureCall(tokens, length, pos))
        return true;

    *pos = current;
    if (stx_conditional(tokens, length, pos))
        return true;

    *pos = current;
    if (stx_while(tokens, length, pos))
        return true;
        
    *pos = current;
    if (stx_for(tokens, length, pos))
        return true;
        
    *pos = current;
    if (stx_read(tokens, length, pos))
        return true;

    *pos = current;

    if (stx_write(tokens, length, pos))
    	return true;
    
	call_exception(tokens, 16, length, pos);	
	return false;
	
}

/// Verifies if the syntax for a composite command is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_compositeCommand(Token* tokens, int length, int* pos) {
    if (!(isReservedWordTypeOf(tokens, length, pos, W_BEGIN))
    	&& isDelimitatorOf(tokens, length, pos, "\n")
        && ! (stx_command(tokens, length, pos))){
        call_exception(tokens, 15, length, pos);
        return false;
}

    loop:
    if (isDelimitatorOf(tokens, length, pos, "\n")) {
    	if (isReservedWordTypeOf(tokens, length, pos, W_END)
			&& isDelimitatorOf(tokens, length, pos, "\n"))
            return true;
        else if (stx_command(tokens, length, pos))
            goto loop;        
    }
    call_exception (tokens, 16, length, pos);
    return false;
}

bool stx_compositeCommand2(Token* tokens, int length, int* pos, char commandType[]) {
    if (isDelimitatorOf(tokens, length, pos, "\n")
		&& ! (stx_command(tokens, length, pos))){
        call_exception(tokens, 15, length, pos);
        return false;
}

    loop:
    if (isDelimitatorOf(tokens, length, pos, "\n")) {
        if ((commandType=="W_IF" && isReservedWordTypeOf(tokens, length, pos, W_ENDIF))
        	|| (commandType=="W_IF" && isReservedWordTypeOf(tokens, length, pos, W_ELSE))
			|| (commandType=="W_WHILE" && isReservedWordTypeOf(tokens, length, pos, W_ENDWHILE))
			|| (commandType=="W_FOR" && isReservedWordTypeOf(tokens, length, pos, W_ENDFOR))	){
			return true;
		}
		else if (stx_command(tokens, length, pos))
            goto loop;
        else
            return true;
    }
    call_exception (tokens, 16, length, pos);
    return false;
}
/// Verifies if the syntax for a block (set of variable declaration, procedures and commands) is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_block(Token* tokens, int length, int* pos) {
    return stx_varPartDeclaration(tokens, length, pos)
        && stx_compositeCommand(tokens, length, pos);
}

/// Verifies if the syntax for an entire program is valid.
///
/// -parameter tokens: Token array pointer.
/// -parameter length: Token array size.
/// -parameter pos: Token array index position.
///
/// - returns: Boolean value indicating if it's a valid syntax.
bool stx_program(Token* tokens, int length, int* pos) {
    bool isReserved = 0, isType = 0, _block = 0, isDelimitatorLine = 0, isDelimitatorDot = 0;

    isReserved = isReservedWordTypeOf(tokens, length, pos, W_PROGRAM);
    
    if(!isReserved){
        call_exception (tokens, 0, length, pos);
    }
	procedures[0]=tokens[*pos-1].value;

     isType = isTypeOf(tokens, length, pos, IDENTIFIER);

     if(!isType && isReserved){
         call_exception (tokens, 1, length, pos);
     }

    isType = true;

    isDelimitatorLine = isDelimitatorOf(tokens, length, pos, "\n");

    if(!isDelimitatorLine && isType && isReserved){
        printf("isDelimitatorLine %d\n", isDelimitatorLine);
        call_exception (tokens, 2, length, pos);
    }

    if (isDelimitatorLine && isType && isReserved){
        _block = stx_block(tokens, length, pos);
        // isDelimitatorDot = isDelimitatorOf(tokens, length, pos, ".");
        isDelimitatorDot = true;
    }

    if(!isDelimitatorDot && _block){
        call_exception (tokens, 3, length, pos);
    }

    printTables();
    
    return isReserved
        && isType
        && isDelimitatorLine
        && _block
        && isDelimitatorDot
        && *pos == length;
}

bool isValidProgram(Token* tokens, int length) {
    list = tokens;
    int pos = 0;
    head = &((Node){ .symbols = NULL, .next = NULL });
    
    return stx_program(tokens, length, &pos);
}
