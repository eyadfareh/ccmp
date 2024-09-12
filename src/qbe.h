#pragma once
#include "ast.h"
#include <stdio.h>

typedef struct QBEProgram QBEProgram;

struct QBEProgram {
	DeclarationList* declarations;
	FILE* f;
	int localCount;
	int shiftWidth;
};

QBEProgram* qbe(DeclarationList* stmts, FILE* f);

void qbe_generate(QBEProgram* p);
