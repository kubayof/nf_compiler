#include<stdio.h>
#include<stdlib.h>

#include "lexer/Types.h"
#include "lexer/Lexer.h"
#include "parser/Types.h"
#include "parser/Parser.h"
#include "binder/Binder.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: <app name> <source code>");
    return 1;
  }

  LexerResult* lexer = tokenizeFile(argv[1]);

  showLexerTokens(lexer);
  
  ParserResult* parser = parse(lexer);
  showTree(parser);

  bindTypes(parser);

  printf("Lexer errors:\n");
  showErrors(lexer->errors);
  printf("Parser errors:\n");
  showErrors(parser->errors);

  printf("Cleaning parse tree...\n");
  clearParserResult(parser);
  printf("Cleaning lexer tokens...\n");
  clearLexerResult(lexer);
  
  return 0;
}
