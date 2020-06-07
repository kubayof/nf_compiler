#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>

#include "Types.h"
#include "Lexer.h"
#include "../errors/Errors.h"

void append(Token*);
Token* newToken(int, int, char*, TokenType type);

Token* tokenRoot = NULL;
Token* last = NULL;
Error* error = NULL;
int line = 0;

void clearRoot() {
  tokenRoot = NULL;
  last = NULL;
  error = NULL;
  line = 0;
}

LexerResult* tokenizeFile(const char* filename) {
  FILE* input = fopen(filename, "r");

  if (input == NULL) {
    printf("Cannot open file with name %s", filename);
    return NULL;
  }
  
  char* inputLine = (char*)malloc(200*sizeof(char));

  while(!(ferror(input) || feof(input))) {
    fgets(inputLine, 200, input);
    line++;
    tokenizeString(inputLine);
  }
  free(inputLine);
  fclose(input);

  LexerResult* result = (LexerResult*)malloc(sizeof(LexerResult));
  result->tokens = tokenRoot;
  result->errors = error;
  clearRoot();
  return result;
}

bool isMultiline = false;

//public
Token* tokenizeString(char* program) {
  int i = 0;

  printf("Tokenizing string: '%s'\n", program);
  
  while (i < strlen(program)) {
    //printf("Symbol: %c index: %d", program[i], i);
    while((program[i] == ' ') || (program[i] == '\t')) {
      i++;
    }

    if (isMultiline == true) {
      while (i < strlen(program)) {
	  if ((program[i] == '*') && (i+1 < strlen(program)) && (program[i+1] == '/')) {
	    i += 2;
	    isMultiline = false;
	  }
	  i++;
	}
	} else if (program[i] == '/') {
      if ((i+1 < strlen(program)) && (program[i+1] == '/')) {
	i++;
	while((i < strlen(program)) && (program[i] != '\n')) {
	  i++;
	}
      } else if ((i+1 < strlen(program) && (program[i+1] == '*'))) {
	i++;
	i++;
	isMultiline = true;
	while (i < strlen(program)) {
	  if ((program[i] == '*') && (i+1 < strlen(program)) && (program[i+1] == '/')) {
	    i += 2;
	    isMultiline = false;
	  }
	  i++;
	}
      } else {
	Token* token = newToken(i, 1, program, SLASH);
	append(token);
	i++;
      }
      } else if (isalpha(program[i])) {
      int start = i;
      i++;
      while(isalpha(program[i]) || isdigit(program[i])) {
	i++;
      }
      int length = i - start;

      
      char* value = (char*)malloc((length + 1) * sizeof(char));
      strncpy(value, program + start, length);
      
      Token* token;
      if (strcmp(value, "int") == 0) {
	token = newToken(start, length, program, INT_TYPE);
      } else if (strcmp(value, "long") == 0) {
	token = newToken(start, length, program, LONG_TYPE);
      } else if (strcmp(value, "float") == 0) {
	token = newToken(start, length, program, FLOAT_TYPE);
      } else if (strcmp(value, "double") == 0) {
	token = newToken(start, length, program, DOUBLE_TYPE);
      } else {
	token = newToken(start, length, program, ID);
      }

      free(value);
      append(token);
    } else if (isdigit(program[i])) {
      int start = i;
      i++;
      while(isdigit(program[i])) {
	i++;
      }
      if (program[i] == '.') {
	i++;
	while(isdigit(program[i])) {
	  i++;
	}
	int length = i - start;
  	Token* token = newToken(start, length, program, DEC_DOUBLE);
	append(token);
      } else {
	int length = i - start;
	bool reverse = (program[start] == '-');
	char* value = (char*) malloc((length + 1) * sizeof(char));
	strncpy(value, program + start + reverse, length - reverse);
	Token* token;
	if (reverse) {
	  int res;
	  if (strlen(value) > 10) {
	    res = 1;
	  } else if (strlen(value) < 10) {
	    res = -1;
	  } else {
	    res = strcmp(value, "2147483648");
	  }
	  if (res <= 0) {
	    token = newToken(start, length, program, DEC_INT);
	  } else {
	    res = strcmp(value, "9223372036854775808");
	    if (res > 0) {
	      char* message = (char*) malloc(50*sizeof(char));
	      sprintf(message, "Number is too small '-%s'", value);
	      error = addError(error, line, i, message);
	    }
	    token = newToken(start, length, program, DEC_LONG);
	  } 
	} else {
	  int res;
	  if (strlen(value) > 10) {
	    res = 1;
	  } else if (strlen(value) < 10) {
	    res = -1;
	  } else {
	    res = strcmp(value, "2147483647");
	  }
	  if (res <= 0) {
	    token = newToken(start, length, program, DEC_INT);
	  } else {
	    if (strlen(value) > 19) {
	      res = 1;
	    } else if (strlen(value) < 19) {
	      res = -1;
	    } else {
	      res = strcmp(value, "9223372036854775807");
	    }
	    if (res > 0) {
	      char* message = (char*) malloc(50*sizeof(char));
	      sprintf(message, "Number is too big '%s'", value);
	      error = addError(error, line, i, message);
	    }
	    token = newToken(start, length, program, DEC_LONG);
	  } 
	}
	free(value);
	append(token);
      }
    } else if (program[i] == '+') {
      Token* token = newToken(i, 1, program, PLUS);
      append(token);
      i++;
    } else if (program[i] == '-') {
      Token* token = newToken(i, 1, program, MINUS);
      append(token);
      i++;
    } else if (program[i] == '*') {
      Token* token = newToken(i, 1, program, STAR);
      append(token);
      i++;
    } else if (program[i] == '(') {
      Token* token = newToken(i, 1, program, OPAR);
      append(token);
      i++;
    } else if (program[i] == ')') {
      Token* token = newToken(i, 1, program, CPAR);
      append(token);
      i++;
    } else if (program[i] == '=') {
      Token* token = newToken(i, 1, program, EQ);
      append(token);
      i++;
    } else if (program[i] == '\n') {
      Token* token = newToken(i, 1, program, NEWLINE);
      append(token);
      i++;
    } else if (program[i] == '{') {
      Token* token = newToken(i ,1, program, OCUR);
      append(token);
      i++;
    } else if (program[i] == '}') {
      Token* token = newToken(i, 1, program, CCUR);
      append(token);
      i++;
    } else if (program[i] == ',') {
      Token* token = newToken(i, 1, program, COMMA);
      append(token);
      i++;
    } else {
      char message[25];
      sprintf(message, "Unexpected symbol: '%c'", program[i]);
      error = addError(error, line, i, message);
      i++;
    }
  }

  return tokenRoot;
}

//public
void showLexerTokens(LexerResult* result) {
  Token* p = result->tokens;
  while (p != NULL) {
    printf("'%d:%d' Type='%d', length='%d' value='%s'\n", p->line, p->position,p->type, p->length, p->value);
    p = p->next;
  }
}

//public
void showLexerErrors(LexerResult *result) {
  showErrors(result->errors);
}

Token* newToken(int position, int length, char*value, TokenType type) {
  Token* token = (Token*)malloc(sizeof(Token));

  token->type = type;
  token->line = line;
  token->position = position;
  token->length = length;
  token->value = (char*)malloc(length + 1);
  strncpy(token->value, value + position, length);
  token->value[length] = '\0';

  return token;
}

void append(Token* token) {
  if (tokenRoot == NULL) {
    tokenRoot = token;
    last = token;
    return;
  }
  last->next = token;
  last = token;
}

void clearLexerResult(LexerResult* result) {
  Token* p;
  Token* head = result->tokens;
  while (head != NULL) {
    p = head->next;
    free(head->value);
    free(head);
    head = p;
  }
  Error* p1;
  Error* head1 = result->errors;
  while (head1 != NULL) {
    p1 = head1->next;
    free(head1->message);
    free(head1);
    head1 = p1;
  }
}
