#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Parser createParser(TokenList tokens) {
  Parser p;
  p.tokens = tokens;
  p.current = 0;
  return p;
}

static Token peek(Parser *p) { return p->tokens.tokens[p->current]; }
static Token previous(Parser *p) { return p->tokens.tokens[p->current - 1]; }
static bool isAtEnd(Parser *p) { return peek(p).type == END_OF_FILE; }
static bool check(Parser *p, TokenType t) { return peek(p).type == t; }
static Token advance(Parser *p) {
  if (!isAtEnd(p))
    p->current++;
  return previous(p);
}
static bool match(Parser *p, TokenType t) {
  if (check(p, t)) {
    advance(p);
    return true;
  }
  return false;
}

static Token consume(Parser *p, TokenType type) {
  if (check(p, type))
    return advance(p);
  fprintf(stderr, "Unexpected Token at line %d '%s' \n", peek(p).line,
          peek(p).lexeme);
  fprintf(stderr, "Expected type %d, got type %d\n", type, peek(p).type);

  exit(1);
}



static Expression *parseExpression(Parser *p);
static Expression* parsePrimary(Parser* p){
  Expression* e = malloc(sizeof(Expression));
  if(match(p, LITERAL)){
    e->type = LITERAL_EXPRESSION;
    e->as.literalExpression.value = atoi(previous(p).lexeme);
  }
  if(match(p, IDENTIFIER)){
    e->type = IDENTIFIER_EXPRESSION;
    e->as.identifierExpression.name = previous(p).lexeme;
  }
  return e;
}

static Expression* finishCall(Parser* p, Expression* e){
  Expression *expr = malloc(sizeof(Expression));
  expr->type = CALL_EXPRESSION;
  expr->as.callExpression.callee = e;
  expr->as.callExpression.parameters = NULL;
  expr->as.callExpression.parametersCount = 0;
  if(match(p, LEFT_PAREN)){
    return expr;
  }
  ExpressionList l;
  l.expressions = malloc(2* sizeof(Expression));
  l.size = 0;
  l.capacity = 2;
  while(!match(p, RIGHT_PAREN)){
    addExpression(&l, parseExpression(p));
    match(p, COMMA);
  }
  expr->as.callExpression.parameters = l.expressions;
  expr->as.callExpression.parametersCount = l.size;
  return expr;
}
static Expression *parseCall(Parser *p) {
  Expression* expr = parsePrimary(p);

  if(match(p, LEFT_PAREN)){
    return finishCall(p, expr);
  }

  return expr;
}
static Expression *parseExpression(Parser *p) {
  return parseCall(p);
}
static Statement *parseStatement(Parser *p) {
  Statement *s = malloc(sizeof(Statement));

  if (match(p, KEYWORD_RETURN)) {
    s->type = RETURN_STATEMENT;
    s->as.returnStatement.expression = parseExpression(p);
    consume(p, SEMICOLON);
  } else {
    s->type = EXPRESSION_STATEMENT;
    s->as.expressionStatement.expression = parseExpression(p);
    consume(p, SEMICOLON);
  }

  return s;
}

static Statement *parseDeclaration(Parser *p) {
  consume(p, KEYWORD_INT);
  Token name = consume(p, IDENTIFIER);

  consume(p, LEFT_PAREN);
  consume(p, RIGHT_PAREN);
  consume(p, LEFT_BRACE);
  Statement *s = malloc(sizeof(Statement));

  StatementList sl;
  sl.capacity = 10;
  sl.size = 0;
  sl.statements = malloc(sl.capacity * sizeof(Statement *));

  while (!match(p, RIGHT_BRACE)) {
    Statement *s = parseStatement(p);
    addStatement(&sl, s);
  }
  s->type = FUNCTION_DECLARATION;
  s->as.functionDeclaration.functionName = name.lexeme;
  s->as.functionDeclaration.parametersCount = 0;
  s->as.functionDeclaration.parameters = NULL;
  s->as.functionDeclaration.bodyCount = sl.size;
  s->as.functionDeclaration.body = sl.statements;
  s->as.functionDeclaration.returnType = "int";

  return s;
}

StatementList parse(Parser *p) {
  StatementList ls;
  ls.size = 0;
  ls.capacity = 10;
  ls.statements = malloc(10 * sizeof(Statement *));
  while (!check(p, END_OF_FILE))
    addStatement(&ls, parseDeclaration(p));
  return ls;
}
