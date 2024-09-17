#pragma once
#include "ast.h"
#include "parser.h"

typedef struct{
  StatementList *statements;
  FILE *f;
  int shiftWidth;
} Program;

Program create_program(StatementList* stmts, FILE* f);
void generate(Program* p);
