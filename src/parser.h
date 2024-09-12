#pragma once
#include "ast.h"
#include "lexer.h"
#include <ctype.h>


typedef struct {
  TokenList tokens;
  int current;
} Parser;

Parser createParser(TokenList);
DeclarationList parse(Parser *p);
