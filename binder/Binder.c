#include<stdlib.h>
#include<stdio.h>

#include "../errors/Errors.h"
#include "../parser/Types.h"
#include "Binder.h"
#include "../symbols/SymbolTable.h"

ExprType bind(Node*);

void bindTypes(ParserResult* result) {
  bind(result->nodes);
}

ExprType bindTerm(Node*);
ExprType bindVar(Node*);
ExprType bindBinaryOp(Node*);
ExprType bindAssignment(Node*);
ExprType bindUnaryOp(Node*);
ExprType bindStmt(Node*);
ExprType bindStmts(Node*);
ExprType bindBlock(Node*);
ExprType bindFunDef(Node*);
ExprType bindFunCall(Node*);
ExprType bindArgs(Node*);
ExprType bindArg(Node*);

ExprType bind(Node* node) {
  switch (node->type) {
  case TERM:
    return bindTerm(node);
  case VAR:
    return bindVar(node);
  case BINARY_OP:
    return bindBinaryOp(node);
  case ASSIGNMENT:
    return bindAssignment(node);
  case UNARY_OP:
    return bindUnaryOp(node);
  case STMT:
    return bindStmt(node);
  case STMTS:
    return bindStmts(node);
  case BLOCK:
    return bindBlock(node);

  case FUN_DEF:
    return bindFunDef(node);
  case FUN_CALL:
    return bindFunCall(node);
  case ARGS:
    return bindArgs(node);
  case ARG:
    return bindArg(node);
  default:
    return UNDEFINED;
  }
}


ExprType bindTerm(Node* node) {
  return node->exprType;
}


ExprType bindVar(Node* node) {
  node->exprType = UNDEFINED;
  return UNDEFINED;
}


ExprType bindBinaryOp(Node* node) {
  ExprType firstType = bind(node->children[0]);
  ExprType secondType = bind(node->children[1]);
  node->exprType = ((firstType > secondType)?firstType:secondType);
  return node->exprType;
}


ExprType bindAssignment(Node* node) {
  node->children[0]->exprType = bind(node->children[1]);
  node->exprType = node->children[0]->exprType;
  return node->exprType;
} 


ExprType bindUnaryOp(Node* node) {
  node->exprType = bind(node->children[0]);
  return node->exprType;
}


ExprType bindStmt(Node* node) {
  if (node == NULL) {
    return UNDEFINED;
  }
  ExprType firstType = bind(node->children[0]);
  ExprType secondType = bind(node->children[1]);
  node->exprType = ((firstType > secondType)?firstType:secondType);

  return node->exprType;
}


ExprType bindStmts(Node* node) {
  node->exprType = bind(node->children[0]);
  return node->exprType;
}


ExprType bindBlock(Node* node) {
  node->exprType = bind(node->children[0]);
  return node->exprType;
}


ExprType bindFunDef(Node* node) {
  node->exprType = bind(node->children[1]);
  return node->exprType;
}
