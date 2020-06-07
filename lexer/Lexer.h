#ifndef LEXER
#define LEXER

#include "Types.h"

//Load from file and tokenize
LexerResult* tokenizeFile(const char* filename);

//Syntax analysis of program
Token* tokenizeString(char *program);

//Show tokoken list
void showLexerTokens(LexerResult* result);

//Show errors
void showLexerErrors(LexerResult* result);

//Clear list
void clearLexerResult(LexerResult* head);

#endif
