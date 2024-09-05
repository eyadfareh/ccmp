#pragma once
#include "ast.h"
#include "lexer.h"
#include <ctype.h>


typedef enum { INT_TYPE } Type;

typedef struct {
  TokenList tokens;
  int current;
} Parser;

Parser createParser(TokenList);
StatementList parse(Parser *p);
