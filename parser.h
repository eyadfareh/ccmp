#pragma once
#include "lexer.h"
#include "ast.h"
#include <ctype.h>

typedef enum {
  INT_TYPE
} Type;

typedef struct {
  TokenList tokens;
  int current;
} Parser;

Parser createParser(TokenList);
Statement* parse(Parser *p);
