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
static bool check(Parser *p, TokenType t) {
  if (isAtEnd(p))
    return false;
  return peek(p).type == t;
}
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
  fprintf(stderr, "ERROR: CRASHING WRONG EVIL BYE BYE\n");
  fprintf(stderr, "%d: %s\n", p->current, p->tokens.tokens[p->current].lexeme);

  exit(1);
}

typedef struct {
  Statement **statements;
  int size;
  int capacity;
} StatementList;

void addStatement(StatementList *sl, Statement *s) {
  if (sl->capacity == sl->size) {
    sl->capacity *= 2;
    sl->statements =
        realloc(sl->statements, sizeof(Statement*) * sl->capacity);
    if (sl->statements == NULL) {
      perror("realloc");
      exit(1);
    }
  }

  sl->statements[sl->size++] = s;
}

static Expression *parseExpression(Parser *p) {
  consume(p, LITERAL);
  Expression *e = (Expression *)malloc(sizeof(Expression));
  e->type = PRIMARY_EXPRESSION;
  e->as.primaryExpression.value = atoi(previous(p).lexeme);
  return e;
}
static Statement *parseStatement(Parser *p) {
  Statement *s = malloc(sizeof(Statement));

  if (match(p, KEYWORD_RETURN)) {
    s->type = RETURN_STATEMENT;
    s->as.returnStatement.expression = parseExpression(p);
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
  sl.statements = malloc(sl.capacity * sizeof(Statement*));

  while (!match(p, RIGHT_BRACE)) {
    Statement *s = parseStatement(p);
    addStatement(&sl, s);
  }
  s->type = FUNCTION_DECLARATION;
  s->as.functionDeclaration.functionName = name.lexeme;
  s->as.functionDeclaration.parameterCount = 0;
  s->as.functionDeclaration.parameters = NULL;
  s->as.functionDeclaration.bodyCount = sl.size;
  s->as.functionDeclaration.body = sl.statements;
  s->as.functionDeclaration.returnType = "int";

  return s;
}

Statement *parse(Parser *p) { return parseDeclaration(p); }
