#pragma once

#include "../lexer/Types.h"
#include "Types.h"

ParserResult* parse(LexerResult* result);

void clearParserResult(ParserResult* result);

void showTree(ParserResult* result);
