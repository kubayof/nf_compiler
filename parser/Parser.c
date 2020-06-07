#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdarg.h>

#include"../errors/Errors.h"
#include "Parser.h"
#include "../lexer/Types.h"
#include "Types.h"


Node* parseTerm();
bool tryMatch(TokenType);
void match(TokenType);
Token* matchOp();
int getBinaryOpPrecedence(Token*);
Node* parseExpression(int);
Node* parseAssignment();
Node* parseStatement();
Node* parseStatements();
Node* parseBlock();
Node* parseArgs();
Node* parseArg();
Node* parseFunDef();
void showNode(Node*);
void next();


Node* parserRoot = NULL;
Error* errors = NULL;
Token* current = NULL;
Token* prev = NULL;
bool logging = true;

int CHILDREN_COUNT=5;

void tryLog(char* format, ...) {
  va_list args;
  if (logging == true) {
    va_start(args, format);
    vfprintf(stdout, format, args);
    printf("\n");
    va_end(args);
  }
}

void clearData() {
  parserRoot = NULL;
  errors = NULL;
  current = NULL;
  prev = NULL;
}

void clearNode(Node* node) {
  for (int i = 0; i < CHILDREN_COUNT; i++) {
    if (node->children[i] != NULL) {
      clearNode(node->children[i]);
    }
  }
  free(node);
}

void clearParserResult(ParserResult* result) {
  clearNode(result->nodes);
  clearErrors(result->errors);
}

void setChildrenNull(Node* node) {
  node->children[0] = NULL;
  node->children[1] = NULL;
}

ParserResult* parse(LexerResult* lexer) {
  current = lexer->tokens;

  ParserResult* result = (ParserResult*) malloc(sizeof(ParserResult));
  result->nodes = parseFunDef();
  if (current != NULL) {
    Error* error = (Error*)malloc(sizeof(error));
    char message[100];
    sprintf(message, "Failed to parse token '%s'", current->value);
    tryLog("Error: %s", message);
    errors = addError(errors, current->line, current->position, message);
  }
  result->errors = errors;
  clearData();
  return result;
}

Node* parseFunDef() {
  tryLog("Starting parsing FUN_DEF...");

  if ((current != NULL) && (current->type == ID)) {
    char* name = current->value;
    Node* args;
    match(ID);
    if ((current != NULL) && (current->type == OPAR)) {
      args = parseArgs();
    } else {
      args = NULL;
    }
    tryMatch(NEWLINE);
    Node* funDef = (Node*) malloc(sizeof(Node));
    funDef->value = name;
    funDef->type = FUN_DEF;
    funDef->children[0] = args;
    funDef->children[1] = parseBlock();
    return funDef;
  }
  return NULL;
}

Node* parseArgs() {
  tryLog("Starting parsing args...");
  match(OPAR);
  Node* args = NULL;

  if ((current != NULL) && (current->type != CPAR)) {
    args = (Node*) malloc(sizeof(Node));
    args->type = ARGS;
    args->children[0] = parseArg();
    Node* cur = args->children[0];
    while ((current != NULL) && (current->type == COMMA)) {
      match(COMMA);
      cur->children[0] = parseArg();
      cur = cur->children[0];
    }
  }
  
  match(CPAR);
  tryLog("Args parsed");
  return args;
}

Node* parseArg() {
  if (current != NULL) {
    ExprType exprType = UNDEFINED;
    if (current->type == INT_TYPE) {
      exprType = INT;
    } else if (current->type == LONG_TYPE) {
      exprType = LONG;
    } else if (current->type == FLOAT_TYPE) {
      exprType = FLOAT;
    } else if (current->type == DOUBLE_TYPE) {
      exprType = DOUBLE;
    } else {
      char* message = (char*) malloc(60 * sizeof(char));
      sprintf(message, "Unexpected token '%s', expected type", current->value);
      errors = addError(errors, current->line, current->position, message);
    }
    next();

    char* value = NULL;
    if ((current != NULL) && (current->type == ID)) {
      value = current->value;
    }
    match(ID);

    Node* arg = (Node*) malloc(sizeof(Node));
    arg->type = ARG;
    arg->value = value;
    arg->exprType = exprType;
    arg->children[0] = NULL;
    
    return arg;
  }
  return NULL;
}

Node* parseBlock() {
  tryLog("Starting parsing BLOCK...");
  
  if (tryMatch(OCUR)) {
    Node* block = (Node*) malloc(sizeof(Node));
    block->type = BLOCK;
    block->value = NULL;
    while ((current != NULL) && (current->type == NEWLINE)) {
      next();
    }
    if ((current != NULL) && (current->type != CCUR)) {
      block->children[0] = parseStatements();
    } else {
      block->children[0] = NULL;
    }
    block->children[1] = NULL;
    match(CCUR);

    tryLog("Block was parsed");
    
    return block;
  } else {
    char* message = (char*) malloc(50*sizeof(char));
    sprintf(message, "Opening curly bracket expected");
    tryLog("Error: %s", message);
    int line;
    int position;
    if (current == NULL) {
      line = prev->line;
      position = prev->position;
    } else {
      line = current->line;
      position = current->position;
    }
    errors = addError(errors, line, position, message);

    printf("Block was not parsed");
    
    return NULL;
  }
}

Node* parseStatements() {
  tryLog("Starting parsing statements...");
  
  Token* lastCurrent = NULL;
  Node* root = NULL;
  Node* last = NULL;
  while ((current != lastCurrent) && (current != NULL) && (current->type != CCUR)) {
    lastCurrent = current;
    Node* temp = parseStatement();
    if (temp != NULL) {
      if (root == NULL) {
	root = temp;
	last = root;
      } else {
	last->children[1] = temp;
	last = last->children[1];
      }
    }
  }

  Node* statements = (Node*) malloc(sizeof(Node));
  statements->type = STMTS;
  statements->value = NULL;
  statements->children[0] = root;
  statements->children[1] = NULL;

  tryLog("Statements parsed...");
  
  return statements;
}

Node* parseStatement() {
  tryLog("Starting parsing statement");

  while ((current != NULL) && (current->type == NEWLINE)) {
    tryLog("Skipping NEWLINE");
    next();
  }
  if (current == NULL) {
    return NULL;
  }
  Node* expression =  parseAssignment();
  /*if (expression == NULL) {
    return NULL;
    }*/
  
  Node* statement = (Node*) malloc(sizeof(Node));
  statement->type = STMT;
  statement->value = NULL;
  statement->children[0] = expression;
  statement->children[1] = NULL;
  if ((current != NULL) && (current->type != CCUR)) {
    match(NEWLINE);
  }

  while((current != NULL) && (current->type == NEWLINE)) {
    match(NEWLINE);
  }

  tryLog("Statement parsed");

  return statement;
}

Node* parseAssignment() {
  Node* root = parseExpression(0);
  Node* lastRoot = NULL;
  Node* last = root;
  while ((current != NULL) && (current->type == EQ)) {
    char* value = current->value;
    next();
    if (last->type != VAR) {
      char* message = (char*) malloc(50 * sizeof(char));
      sprintf(message, "Left part of assignment must be an id");
      tryLog("Error: %s", message);
      errors = addError(errors, prev->line, prev->position, message);
    }
    Node* child = parseExpression(0);
    Node* eqNode = (Node*)malloc(sizeof(Node));
    eqNode->type = ASSIGNMENT;
    eqNode->value = value;
    eqNode->children[0] = last;
    eqNode->children[1] = child;
    if (lastRoot != NULL) {
      lastRoot->children[1] = eqNode;
      lastRoot = last;
      last = child;
    } else {
      root = eqNode;
      lastRoot = root;
      last = child;
    }
  }
  
  if (root == NULL) {
    
  } else {
    tryLog("Returning right expression: ");
    if (logging == true) {
      showNode(root);
    }
    tryLog("\n\n");
  }
    
  return root;
}

int PARENTHESES=0;
int ident = 0;
Node* parseExpression(int prevPrecedence) {
  ident++;
  Node* left = parseTerm();
  
  if (left == NULL) {
    tryLog("%d: Left parsed: 'NULL'", ident);
    if (current != NULL) {
      char* message = (char*) malloc(60 * sizeof(char));
      sprintf(message, "Unexpected token '%s'", current->value);
      tryLog("Error: %s", message);
      errors = addError(errors, current->line, current->position, message);
    }
  } else {
    tryLog("%d: Left parsed: '%s'", ident, left->value);
  }
  Node* node = NULL;
  if (PARENTHESES > 0) {
    while (tryMatch(NEWLINE));
  }
  int curPrecedence = getBinaryOpPrecedence(current);
  while(curPrecedence > prevPrecedence) {
    Token* operator = matchOp();
    if (operator != NULL) {
      tryLog("%d: Operator: '%s'", ident, operator->value);
    } else {
      tryLog("%d: Operator: 'NULL'", ident);
    }
    while (tryMatch(NEWLINE));
    
    node = (Node*) malloc(sizeof(Node));
    node->type = BINARY_OP;
    node->value = operator->value;

    node->children[0] = left;
    left = node;

    node = parseExpression(curPrecedence);
    if (node == NULL) {
      tryLog("%d: Right parsed: 'NULL'", ident);
      char* message = (char*)malloc(60 * sizeof(char));
      sprintf(message, "Cannot find second operand for operator '%s'", operator->value);
      tryLog("Error: %s\n", message);
      errors = addError(errors, operator->line, operator->position, message);
    } else {
      tryLog("%d: Right parsed: '%s'", ident, node->value);
    }
    left->children[1] = node;
    curPrecedence = getBinaryOpPrecedence(current);
  }

  if (left == NULL) {
    tryLog("%d: Returning 'NULL'", ident);
  } else {
    tryLog("%d: Returning ", ident);
    if (logging == true) {
      showNode(left);
    }
    tryLog("\n\n");
  }
  ident--;
  return left;
}

Node* parseTerm() {/*
  if (tryMatch(ID)) {
    Node* term = (Node*)malloc(sizeof(Node));
    term->type = VAR;
    term->value = prev->value;
    setChildrenNull(term);
    return term;
    }*/

  if (current == NULL) {
    char* message = (char*) malloc(30*sizeof(char));
    sprintf(message, "Unexpected end of file");
    errors = addError(errors, prev->line, prev->position, message);
    return NULL;
  }
  
  if (tryMatch(OPAR)) {
    PARENTHESES++;
    while(tryMatch(NEWLINE));
    Node* result = parseAssignment();
    match(CPAR);
    PARENTHESES--;
    return result;
  }

  Node* term = (Node*)malloc(sizeof(Node));
  term->type = TERM;
  term->value = current->value;
  setChildrenNull(term);
  
  if (tryMatch(DEC_INT)) {
    term->exprType = INT;
  } else if (tryMatch(DEC_LONG)) {
    term->exprType = LONG;
  } else if (tryMatch(DEC_FLOAT)) {
    term->exprType = FLOAT;
  } else if (tryMatch(DEC_DOUBLE)) {
    term->exprType = DOUBLE;
  } else if (tryMatch(ID)) {
    term->type = VAR;
  } else {
    free(term);
    return NULL;
  }
  
  return term;
}


void next() {
  if (current == NULL) {
    const char* constMessage = "Unexpected end of file during next";
    char* message = (char*) malloc(strlen(constMessage) + 1);
    strcpy(message, constMessage);
    tryLog("Error: %s", message);
    errors = addError(errors, prev->line, prev->position, message);
    return;
  }
  prev = current;
  current = current->next;
}

Token* matchOp() {
  if (current == NULL) {
    const char* constMessage = "Unexpected end of file";
    char* message = (char*) malloc(strlen(constMessage) + 1);
    strcpy(message, constMessage);
    tryLog("Error: %s", message);
    errors = addError(errors, prev->line, prev->position, message);
    return NULL;
  }
  if ((current->type != STAR) &&
      (current->type != SLASH) &&
      (current->type != PLUS) &&
      (current->type != MINUS)) {
    const char* constMessage =  "Unexpected token";
    char* message = (char*) malloc(strlen(constMessage) + 1);
    strcpy(message, constMessage);
    tryLog("Error: %s", message);
    errors = addError(errors, prev->line, prev->position, message);
  }
  Token* token = current;
  next();
  return token;
}

void match(TokenType type) {
  if (current == NULL) {
    const char* constMessage = "Unexpected end of file: cannot match";
    char* message = (char*) malloc(strlen(constMessage) + 1);
    strcpy(message, constMessage);
    tryLog("Error: %s", message);
    errors = addError(errors, prev->line, prev->position, message);
    return;
  }
  if (current->type != type) {
    const char* constMessage = "Unexpected token: cannot match";
    char* message = (char*) malloc(strlen(constMessage) + 1);
    strcpy(message, constMessage);
    tryLog("Error: %s", message);
    errors = addError(errors, prev->line, prev->position, message);
    return;
  }
  next();
}

bool tryMatch(TokenType type) {
  if ((current == NULL) ||
      (current->type != type)){
    return false;
  }
  next();
  return true;
}

int getBinaryOpPrecedence(Token* op) {
  if (op == NULL) {
    return 0;
  }
  switch (op->type) {
  case STAR:
    return 2;
  case SLASH:
    return 2;
  case PLUS:
    return 1;
  case MINUS:
    return 1;
  default:
    return 0;
  }
}

void showNode(Node* node) {
  if (node == NULL) {
    printf("!!!");
    return;
  }

  if (node->type == FUN_DEF) {
    printf("(FUN ");
    printf("%s ", node->value);
    showNode(node->children[0]);
    printf(" ");
    showNode(node->children[1]);
    printf(")");
    return;
  }

  if (node->type == ARGS) {
    printf("(ARGS ");
    if (node->children[0] != NULL) {
      showNode(node->children[0]);
    }
    printf(")");
    return;
  }

  if (node->type == ARG) {
    printf("(");
    switch (node->exprType) {
    case INT:
      printf("INT ");
      break;
    case LONG:
      printf("LONG ");
      break;
    case FLOAT:
      printf("FLOAT ");
      break;
    case DOUBLE:
      printf("DOUBLE ");
      break;
    default:
      printf("NOTYPE ");
      break;
    }
    printf("%s)", node->value);

    if (node->children[0] != NULL) {
      printf(" ");
      showNode(node->children[0]);
    }
    return;
  }

  if (node->type == STMTS) {
    printf("(STMTS");
    if (node->children[0] != NULL) {
      showNode(node->children[0]);
    }
    printf(")");
    return;
  }

  if (node->type == BLOCK) {
    printf("(BLOCK");
    if (node->children[0] != NULL) {
      printf(" ");
      showNode(node->children[0]);
    }
    printf(")");
    return;
  }

  if (node->type == STMT) {
    printf(" ");
    if (node->children[0] != NULL) {
      showNode(node->children[0]);
    } else {
      printf("NULL");
    }
    if (node->children[1] != NULL) {
      showNode(node->children[1]);
    } else {
      printf("LAST");
    }
    return;
  }
  
  if ((node->children[0] == NULL) &&
      (node->children[1] == NULL)) {
    printf("%s", node->value);
    return;
  }
  printf("(%s ", node->value);
  if (node->children[0] != NULL) {
    showNode(node->children[0]);
  }
  if (node->children[1] != NULL) {
    printf(" ");
    showNode(node->children[1]);
  }
  printf(")");
}

void showTree(ParserResult* result) {
  showNode(result->nodes);
  printf("\n");
}
