#pragma once

#include "../errors/Errors.h"

typedef enum NodeType {
		       TERM=0,//children[0]=NULL, children[1]=NULL, value - term
		       VAR=1,//subtype of term
		       BINARY_OP=2,//children[0]=op|term, children[1]=op|term, value - operator
		       ASSIGNMENT=3,//children[0]=term, children[1]=op|term, value - "="
		       UNARY_OP=4,//children[0]=op|term, children[1]=NULL, value - operator
		       STMT=5,//children[0]=op|term|assign, children[1] - pointer to next stmt
		       STMTS=6,//children[0]=stmt, children[1]=NULL
		       BLOCK=7,//children[0]=stmts, children[1]=NULL
		       
		       FUN_DEF=8,//value - fun name, ch[0] - args, ch[1] - block
		       FUN_CALL=9,//value - fun name, ch[0] - args
		       ARGS=10,//ch[0] - arg
		       ARG=11,//value - var name, exprType - type of var, ch[0] - next arg

} NodeType;

typedef enum ExprType {
		       UNDEFINED=0,
		       INT=1,//4 bytes
		       LONG=2,//8 bytes
		       FLOAT=3,//4 bytes
		       DOUBLE=4,//8 bytes
		       CLASS=5//8 bytes
} ExprType;

//IMPORTANT: values are skipped during clean, they are cleaned with lexer
typedef struct Node {
  NodeType type;
  ExprType exprType;
  char* value;
  struct Node* children[5];
} Node;


typedef struct ParserResult {
  Node* nodes;
  Error* errors;
} ParserResult;
