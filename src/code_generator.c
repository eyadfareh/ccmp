#include "code_generator.h"
#include "ast.h"
#include <stdarg.h>
#include <string.h>

void emit(Program *p, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(p->f, fmt, args);
  va_end(args);
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
void emitLineShift(Program *p) {
  if (p->shiftWidth > 0)
    emit(p, "%0*c", p->shiftWidth * 2, ' ');
}
void emitLine(Program *p, char *fmt, ...) {
  emitLineShift(p);
  va_list args;
  va_start(args, fmt);
  vfprintf(p->f, fmt, args);
  va_end(args);
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

Program create_program(StatementList *stmts, FILE *f) {
  Program p;
  p.statements = stmts;
  p.f = f;
  p.shiftWidth = 0;
  return p;
}

void emitStatement(Program *p, Statement *s);
void emitExpression(Program *p, Expression *s);
void emitType(Program *p, Type *t);

void emitType(Program *p, Type *t) {
  if (t->type == BASIC_TYPE) {
    if (strcmp(t->as.basicType.name, "int") == 0) {
      emit(p, "int");
    }else if (strcmp(t->as.basicType.name, "void") == 0) {
      emit(p, "void");
    }
  }
}

void emitBinaryExpression(Program *p, Expression *s) {
  emit(p, "(");
  emitExpression(p, s->as.binaryExpression.left);
  emit(p, ")");
  switch (s->as.binaryExpression.operator) {
  case PLUS:
    emit(p, " + ");
    break;
  case MINUS:
    emit(p, " - ");
    break;
  case STAR:
    emit(p, " * ");
    break;
  case SLASH:
    emit(p, " / ");
    break;
  case EQUAL_EQUAL:
    emit(p, " == ");
    break;
  case EQUAL:
    emit(p, " = ");
    break;
  }
  emit(p, "(");
  emitExpression(p, s->as.binaryExpression.right);
  emit(p, ")");
}
void emitCallExpression(Program *p, Expression *s) {
  emit(p, "%s(", s->as.callExpression.callee->as.identifierExpression.name);
  for (int i = 0; i < s->as.callExpression.parametersCount; i++) {
    emitExpression(p, s->as.callExpression.parameters[i]);
    if (i != s->as.callExpression.parametersCount - 1) {
      emit(p, ", ");
    }
  }
  emit(p, ")");
}
void emitExpression(Program *p, Expression *s) {
  switch (s->type) {
  case IDENTIFIER_EXPRESSION:
    emit(p, "%s", s->as.identifierExpression.name);
    break;
  case LITERAL_EXPRESSION:
    emit(p, "%d", s->as.literalExpression.value);
    break;
  case STRING_EXPRESSION:
    emit(p, "%s", s->as.stringExpression.value);
    break;
  case CALL_EXPRESSION:
    emitCallExpression(p, s);
    break;
  case BINARY_EXPRESSION:
    emitBinaryExpression(p, s);
    break;
  }
}
void emitCompoundStatement(Program *p, Statement *s) {
  emit(p, "{\n");
  p->shiftWidth++;
  for (int i = 0; i < s->as.compoundStatement.statementsCount; i++) {
    emitStatement(p, s->as.compoundStatement.statements[i]);
  }
  p->shiftWidth--;
  emitLineShift(p);
  emit(p, "}\n");
}
void emitVariableDeclaration(Program *p, Statement *s) {
  emitLineShift(p);
  emitType(p, s->as.variableDeclaration.type);
  emit(p, " %s", s->as.variableDeclaration.name);
  if (s->as.variableDeclaration.initializer != NULL) {
    emit(p, " = ");
    emitExpression(p, s->as.variableDeclaration.initializer);
  }
  emit(p, ";\n");
}
void emitIfStatement(Program *p, Statement *s) {
  emitLineShift(p);
  emit(p, "if (");
  emitExpression(p, s->as.ifStatement.condition);
  emit(p, ") ");
  emitStatement(p, s->as.ifStatement.body);
  if (s->as.ifStatement.elseBody != NULL) {
    emitLineShift(p);
    emit(p, "else ");
    emitStatement(p, s->as.ifStatement.elseBody);
  }
}
void emitStatement(Program *p, Statement *s) {
  switch (s->type) {
  case COMPOUND_STATEMENT:
    emitCompoundStatement(p, s);
    break;
  case RETURN_STATEMENT:
    emitLine(p, "return ");
    emitExpression(p, s->as.returnStatement.expression);
    emit(p, ";\n");
    break;
  case EXPRESSION_STATEMENT:
    emitLineShift(p);
    emitExpression(p, s->as.expressionStatement.expression);
    emit(p, ";\n");
    break;
  case VARIABLE_DECLARATION:
    emitVariableDeclaration(p, s);
    break;
  case IF_STATEMENT:
    emitIfStatement(p, s);
    break;
  default:
    emitLine(p, "// Unknown statement type %d\n", s->type);
    break;
  }
}
void emitFunctionDefenition(Program *p, Statement *s) {
  emitLineShift(p);
  emitType(p, s->as.functionDefinition.returnType);
  emit(p, " %s(", s->as.functionDefinition.functionName);
  for (int i = 0; i < s->as.functionDefinition.parametersCount; i++) {
    emitType(p, s->as.functionDefinition.parameterTypes[i]);
    emit(p, " %s", s->as.functionDefinition.parameters[i]);
    if (i != s->as.functionDefinition.parametersCount - 1) {
      emit(p, ", ");
    }
  }
  emit(p, ") ");
  emitStatement(p, s->as.functionDefinition.body);
}
void generate(Program *p) {
  emitLine(p, "#include <stdio.h>\n");
  emitLine(p, "#include <stdlib.h>\n");
  for (int i = 0; i < p->statements->size; i++) {
    switch (p->statements->statements[i]->type) {
    case FUNCTION_DEFINITION:
      emitFunctionDefenition(p, p->statements->statements[i]);
      break;
    }
  }
}
