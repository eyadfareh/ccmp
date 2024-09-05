#pragma once
#include "ast.h"
#include <stdio.h>

typedef struct QBEProgram QBEProgram;

struct QBEProgram {
	StatementList* stmts;
	FILE* f;
	int localCount;
	int shiftWidth;
};

QBEProgram* qbe(StatementList* stmts, FILE* f);

void qbe_generate(QBEProgram* p);
