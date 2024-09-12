#include "qbe.h"
#include "ast.h"
#include "lexer.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


QBEProgram *qbe(DeclarationList *decls, FILE *f) {
  QBEProgram *p = malloc(sizeof(QBEProgram));
  p->declarations = decls;
  p->f = f;
  p->localCount = 0;
  return p;
}

// wrapper around fprintf
static void emit(QBEProgram *p, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(p->f, fmt, args);
  va_end(args);
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

static void emitLineShift(QBEProgram *p) {
  emit(p, "%0*c", p->shiftWidth * 2, ' ');
}
static void emitStatement(QBEProgram *p, Statement *s);
static int emitExpression(QBEProgram *p, Expression *s);

static void emitReturnStatement(QBEProgram *p, Statement *s) {
  int x = emitExpression(p, s->as.returnStatement.expression);
  emitLineShift(p);
  emit(p, "ret %%p%d\n", x);
}
static void emitCompoundStatement(QBEProgram *p, Statement *s) {
	for (int i = 0; i < s->as.compoundStatement.statementsCount; i++) {
		emitStatement(p, s->as.compoundStatement.statements[i]);
	}
}
static void emitFunctionDefinition(QBEProgram *p, Declaration *s) {
  p->localCount = 0;
  struct FunctionDefinition fd = s->as.functionDefinition;
  emit(p, "export function w $%s(){\n", fd.functionName);
  emit(p, "@start\n");
  p->shiftWidth++;
	emitCompoundStatement(p, fd.body);
  p->shiftWidth--;
  emit(p, "}\n");
}
static void emitStatement(QBEProgram *p, Statement *s) {
  switch (s->type) {
  case RETURN_STATEMENT:
    emitReturnStatement(p, s);
    break;
  case EXPRESSION_STATEMENT:
    emitExpression(p, s->as.expressionStatement.expression);
    break;
  default:
    emit(p, "# Unknown statement type %d\n", s->type);
  }
}
static int emitBinaryExpression(QBEProgram *p, Expression *e) {
	int left = emitExpression(p, e->as.binaryExpression.left);
	int right = emitExpression(p, e->as.binaryExpression.right);
	emitLineShift(p);
	switch(e->as.binaryExpression.operator){
		case PLUS:
			emit(p, "%%p%d = w add %%p%d, %%p%d\n", p->localCount, left, right);
			break;
		case MINUS:
			emit(p, "%%p%d = w sub %%p%d, %%p%d\n", p->localCount, left, right);
			break;
		case STAR:
			emit(p, "%%p%d = w mul %%p%d, %%p%d\n", p->localCount, left, right);
			break;
		case SLASH:
			emit(p, "%%p%d = w div %%p%d, %%p%d\n", p->localCount, left, right);
			break;
	}
	return p->localCount++;

}
static int emitExpression(QBEProgram *p, Expression *s) {
  switch (s->type) {
  case LITERAL_EXPRESSION:
    emitLineShift(p);
    emit(p, "%%p%d = w copy %d\n", p->localCount, s->as.literalExpression.value);
    return p->localCount++;
  case CALL_EXPRESSION: {
    int *ps = malloc(sizeof(int) * s->as.callExpression.parametersCount);
    for (int i = 0; i < s->as.callExpression.parametersCount; i++) {
      ps[i] = emitExpression(p, s->as.callExpression.parameters[i]);
    }
    emitLineShift(p);
    emit(p, "%%p%d = w call $%s(", p->localCount,
         s->as.callExpression.callee->as.identifierExpression.name);

    for (int i = 0; i < s->as.callExpression.parametersCount; i++) {
      emit(p, "w %%p%d", ps[i]);
      if (i != s->as.callExpression.parametersCount - 1) {
        emit(p, ", ");
      }
    }
    emit(p, ")\n");

    free(ps);
    return p->localCount++;
  }
	case BINARY_EXPRESSION:
		return emitBinaryExpression(p, s);
  }
}
void emitBloat(QBEProgram *p) {
  emit(p, "function w $printInt(w %%a){\n"
          "@start\n"
          "  call $printf(l $fmt, ..., w %%a)\n"
          "  ret 0\n"
          "}\n");
  emit(p, "export function w $_start() {\n"
          "@start\n"
          "  %%e = w call $main()\n"
          "  call $exit(w %%e)\n"
          "  ret 0\n"
          "}\n");
  emit(p, "data $fmt = {b \"%%d\\n\"}\n");
}
void qbe_generate(QBEProgram *p) {

  for (int i = 0; i < p->declarations->size; i++) {
    switch (p->declarations->declarations[i]->type) {
    case FUNCTION_DEFINITION:
      emitFunctionDefinition(p, p->declarations->declarations[i]);
      break;
    default:
      perror("unknown statement type\n");
      perror("qbe.c:qbe_generate()\n");
      exit(1);
    }
  }

  emitBloat(p);
}
