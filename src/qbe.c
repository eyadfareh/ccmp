#include "qbe.h"
#include "ast.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

QBEProgram *qbe(StatementList *stmts, FILE *f) {
  QBEProgram *p = malloc(sizeof(QBEProgram));
  p->stmts = stmts;
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

static void emitLineShit(QBEProgram *p) {
  emit(p, "%0*c", p->shiftWidth * 2, ' ');
}
static void emitStatement(QBEProgram *p, Statement *s);
static int emitExpression(QBEProgram *p, Expression *s);

static void emitReturnStatement(QBEProgram *p, Statement *s) {
  int x = emitExpression(p, s->as.returnStatement.expression);
  emitLineShit(p);
  emit(p, "ret %%p%d\n", x);
}
static void emitFunctionDeclaration(QBEProgram *p, Statement *s) {
  p->localCount = 0;
  struct FunctionDeclaration fd = s->as.functionDeclaration;
  emit(p, "export function w $%s(){\n", fd.functionName);
  emit(p, "@start\n");
  p->shiftWidth++;
  for (int i = 0; i < fd.bodyCount; i++) {
    emitStatement(p, fd.body[i]);
  }
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
static int emitExpression(QBEProgram *p, Expression *s) {
  switch (s->type) {
  case LITERAL_EXPRESSION:
    emitLineShit(p);
    emit(p, "%%p%d =w copy %d\n", p->localCount, s->as.literalExpression.value);
    return p->localCount++;
  case CALL_EXPRESSION: {
    int *ps = malloc(sizeof(int) * s->as.callExpression.parametersCount);
    for (int i = 0; i < s->as.callExpression.parametersCount; i++) {
      ps[i] = emitExpression(p, s->as.callExpression.parameters[i]);
    }
    emitLineShit(p);
    emit(p, "%%p%d = w call $%s(", p->localCount,
         s->as.callExpression.callee->as.identifierExpression.name);

    for (int i = 0; i < s->as.callExpression.parametersCount; i++) {
      emit(p, "w %d", ps[i]);
      if (i != s->as.callExpression.parametersCount - 1) {
        emit(p, ", ");
      }
    }
    emit(p, ")\n");

    free(ps);
    return p->localCount++;
  }
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

  for (int i = 0; i < p->stmts->size; i++) {
    switch (p->stmts->statements[i]->type) {
    case FUNCTION_DECLARATION:
      emitFunctionDeclaration(p, p->stmts->statements[i]);
      break;
    default:
      perror("unknown statement type\n");
      perror("qbe.c:qbe_generate()\n");
      exit(1);
    }
  }

  emitBloat(p);
}
