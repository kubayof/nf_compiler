#pragma once

typedef struct Error {
  int line;
  int position;
  char* message;

  struct Error* next;
} Error;


Error* addError(Error* root, int line, int position, const char* message);

void showErrors(Error* root);

void clearErrors(Error* root);
