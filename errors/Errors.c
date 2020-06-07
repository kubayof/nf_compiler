#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "Errors.h"

Error* addError(Error* root, int line, int position, const char* message) {
  Error* error = (Error*)malloc(sizeof(Error));

  error-> line = line;
  error->position = position;
  error->message = (char*)malloc(strlen(message) + 1);
  strcpy(error->message, message);
  error->next = NULL;

  Error* p = root;
  if (p != NULL) {
    while(p->next != NULL) {
      p = p->next;
    }
    p->next = error;
    return root;
  } else {
    return error;
  }
}

void showErrors(Error* root) {
  Error* p = root;
  while(p != NULL) {
    printf("%d:%d -> %s\n", p->line, p->position, p->message);
    p = p->next;
  }
}

void clearErrors(Error* error) {
  Error* p = error;
  while(p != NULL) {
    free(error->message);
    error = p;
    p = p->next;
    free(error);
  }
}

