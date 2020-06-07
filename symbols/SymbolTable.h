#pragma once

#include "../parser/Types.h"

// Not works for classes yet
//
//
//
//

typedef struct Symbol {
  char* name;
  ExprType type;
  char* className;//not NULL if type=CLASS
  struct Symbol* next;
} Symbol;

typedef struct Scope {
  Symbol* symbols;
  struct Scope* next;
} Scope;

typedef struct Argument {
  ExprType type;
  struct Argument* next;
} Argument;

typedef struct Function {
  char* name;
  Argument* args;
  struct Function* next;
} Function;

typedef struct SymbolTable {
  Function* functions;
  Scope* scope;
} SymbolTable;

SymbolTable* newSymbolTable();

void pushScope(SymbolTable* table);

void popScope(SymbolTable* table);

//Search for variables in all the scopes, retyurns NULL if not found
Symbol* findVariable(SymbolTable* table, char* variable);

//Find by call or by definition Node
Function* findFunction(SymbolTable* table,  Node* funCall);

//Assuming that SymbolTable->scope != NULL
//Returns NULL if variable is already defined in current scope
//term - TERM or VAR
Symbol* defineVar(SymbolTable* table, Node* term);

//Returns NULL if function in already defined
Function* defineFunction(SymbolTable* table, Node* funDef);

void clearSymbolTable(SymbolTable* table);
