#include<stdlib.h>
#include<stdio.h>

#include "Parser.h"
#include "../lexer/Types.h"
#include "Types.h"

Node* parserRoot = NULL;
Token* current = NULL;
Error* errors = NULL;

void clearData() {
  parserRoot = NULL;
  errors = NULL;
  current = NULL;
}

ParserResult* parse(LexerResult* result) {


  clearData();
  return NULL;
}


Node* parsePrimaryExpression() {

  return NULL;
}

Node* parseFactor() {

  return NULL;
}

Node* parseTerm() {
  if ((current->type == ID) ||
      (current->type == DEC_INT) ||
      (current->type == DEC_FLOAT)) {
    Node* term = (Node*)malloc(sizeof(Node));
    term->value = current->value;
    return term;
  }

  if (current->type == OPAR) {
    current = current->next;
    Node* result = parsePrimaryExpression();
    if (current->type == CPAR) {
      current = current->next;
    } else {
      //Error* error = 
    }
  }
  return NULL;
}
