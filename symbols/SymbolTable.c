#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "SymbolTable.h"

SymbolTable* newSymbolTable() {
  SymbolTable* table = (SymbolTable*) malloc(sizeof(SymbolTable));
  table->functions = NULL;
  table->scope = NULL;

  return table;
}


void pushScope(SymbolTable* table) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->symbols = NULL;
  scope->next = table->scope;
  table->scope = scope;
}


void popScope(SymbolTable* table) {
  if (table->scope == NULL) {
    printf("Attempt to pop NULL sope\n");
    return;
  }

  Scope* scope = table->scope;
  table->scope = scope->next;

  free(scope);
}


Symbol* findVariable(SymbolTable* table, char* variable) {
  Scope* scope = table->scope;
  while (scope != NULL) {
    Symbol* symbol = scope->symbols;
    while (symbol != NULL) {
      if (strcmp(symbol->name, variable) == 0) {
	return symbol;
      }
      symbol = symbol->next;
    }
    scope = scope->next;
  }

  return NULL;
}


Function* findFunction(SymbolTable* table, Node* funCall) {
  Function* function = table->functions;
  while (function != NULL) {
    if (strcmp(function->name, funCall->value)) {
      Argument* arg = function->args;
      Node* argNode = funCall->children[1];
      if (argNode != NULL) {
	argNode = argNode->children[0];
      }
      while((arg != NULL) && (argNode != NULL)) {
	if (arg->type != argNode->exprType) {
	  break;
	}
	arg = arg->next;
	argNode = argNode->children[0];
      }
      if ((arg == NULL) && (argNode == NULL)) {
	return function;
      }
    }
    function = function->next; 
  }

  return NULL;
}


Symbol* defineVar(SymbolTable* table, Node* term) {
  Symbol* var = table->scope->symbols;
  while (var != NULL) {
    if (strcmp(var->name, term->value) == 0) {
      return NULL;
    }
    var = var->next;
  }
  
  Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
  symbol->name = term->value;
  symbol->type = term->exprType;
  //Place for logic related to class processing
  symbol->next = table->scope->symbols;
  table->scope->symbols = symbol;

  return symbol;
}

Function* defineFunction(SymbolTable* table, Node* funDef) {
  if (findFunction(table, funDef) != NULL) {
    return NULL;
  }

  Function* function = (Function*) malloc(sizeof(Function));
  function->name = funDef->value;
  if ((funDef->children[0] == NULL) || (funDef->children[0]->children[0] = NULL)) {
    function->args = NULL;
  } else {
    Node* arg = funDef->children[0]->children[0];
    function->args = NULL;
    Argument* argument = NULL; 
    while(arg != NULL) {
      Argument* a = (Argument*) malloc(sizeof(Argument));
      a->type = arg->exprType;
      a->next = NULL;
      if (argument == NULL) {
	argument = a;
	function->args = a;
      } else {
	argument->next = a;
	argument = a; 
      }
      arg = arg->children[0];
    }
  }
  function->next = table->functions;
  table->functions = function;

  return function;
}

void clearArgs(Argument* arg) {
  if (arg == NULL) {
    return;
  }

  clearArgs(arg->next);
  free(arg);
}

void clearFunctions(Function* function) {
  if (function == NULL) {
    return;
  }

  clearFunctions(function->next);
  clearArgs(function->args);
  free(function);
}

void clearSymbols(Symbol* symbol) {
  if (symbol == NULL) {
    return;
  }

  clearSymbols(symbol->next);
  free(symbol);
}

void clearScopes(Scope* scope) {
  if (scope == NULL) {
    return;
  }

  clearScopes(scope->next);
  clearSymbols(scope->symbols);
  free(scope);
}

void clearSymbolTable(SymbolTable* table) {
  clearFunctions(table->functions);
  clearScopes(table->scope);
}
