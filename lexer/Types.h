#ifndef LEXER_TYPES
#define LEXER_TYPES

#include "../errors/Errors.h"

typedef enum TokenType {
			ID=0,
			PLUS=1,
			MINUS=2,
			STAR=3,
			SLASH=4,
			OPAR=5,
			CPAR=6,
			EQ=7,
			NEWLINE=8,
			OCUR=9,
			CCUR=10,
			COMMA=11,

			DEC_INT=12,
			DEC_LONG=13,
			DEC_DOUBLE=14,
			DEC_FLOAT=15,

			INT_TYPE=16,
			LONG_TYPE=17,
			FLOAT_TYPE=18,
			DOUBLE_TYPE=19
} TokenType;

typedef struct Token {
  TokenType type;
  int line;
  int position;
  int length;
  char *value;
  
  struct Token *next;
} Token;

typedef struct LexerResult {
  Token* tokens;
  Error* errors;
} LexerResult;

#endif
