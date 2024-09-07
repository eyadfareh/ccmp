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

static Expression* finishCall(Parser* p, Expression* e){
	Expression *expr = createCallExpression(e, NULL, 0);
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

// TODO: complete
static Expression* parsePrimaryExpression(Parser* p){
  Expression* e = createEmptyExpression();
  if(match(p, LITERAL)){
    e->type = LITERAL_EXPRESSION;
    e->as.literalExpression.value = atoi(previous(p).lexeme);
  }else if(match(p, IDENTIFIER)){
    e->type = IDENTIFIER_EXPRESSION;
    e->as.identifierExpression.name = previous(p).lexeme;
  }else if(match(p, LEFT_PAREN)){
		e = parseExpression(p);
		match(p, RIGHT_PAREN);
  }
  return e;
}

// TODO: complete
static Expression *parsePostfixExpression(Parser *p) {
  Expression* expr = parsePrimaryExpression(p);

	while(match(p, LEFT_PAREN)){
		switch(previous(p).type){
			case LEFT_PAREN:
				return finishCall(p, expr);
			default:
				exit(0x285);
				return NULL;
		}
	}
  return expr;
}
// TODO: complete
static Expression *parseUnaryExpression(Parser *p) {
	Expression* e = parsePostfixExpression(p);
	if(match(p, PLUS_PLUS) || match(p, MINUS_MINUS)){
		return createUnaryExpression(previous(p).type, e);
	}else if(match(p, MINUS) || match(p, TILDE) || match(p, BANG) || match(p, PLUS) || match(p, STAR) || match(p, AND)){
		return createUnaryExpression(previous(p).type, e);
	}
	return e;
}
// TODO: complete
static Expression *parseCastExpression(Parser *p) { return parseUnaryExpression(p); }

// done
static Expression *parseMultiplicativeExpression(Parser *p) { 
	Expression* e = parseCastExpression(p);
	while(match(p, STAR) || match(p, SLASH) || match(p, PERCENT)){
		e = createBinaryExpression(e, previous(p).type, parseCastExpression(p));
	}
	return e;
}
// done
static Expression *parseAdditiveExpression(Parser *p) {
	Expression* e = parseMultiplicativeExpression(p);
	while(match(p, PLUS) || match(p, MINUS)){
		e = createBinaryExpression(e, previous(p).type, parseMultiplicativeExpression(p));
	}
	return e;
}

// TODO: complete
static Expression *parseShiftExpression(Parser *p) {
	Expression *e = parseAdditiveExpression(p);
	return e;
}

// done
static Expression *parseRelationalExpression(Parser *p){
	Expression *e = parseShiftExpression(p);
	while(match(p, LESS_THAN) || match(p, GREATER_THAN) || match(p, LESS_EQUAL) || match(p, GREATER_EQUAL) ){
		e = createBinaryExpression(e, previous(p).type, parseShiftExpression(p));
	}
	return e;
}

// done
static Expression *parseEqualityExpression(Parser *p){
	Expression *e = parseRelationalExpression(p);
	while(match(p, BANG_EQUAL) || match(p, EQUAL_EQUAL)){
		e = createBinaryExpression(e, previous(p).type, parseRelationalExpression(p));
	}
	return e;
}

// done
static Expression *parseAndExpression(Parser *p){
	Expression *e = parseEqualityExpression(p);
	while(match(p, AND)){
		e = createBinaryExpression(e, AND, parseEqualityExpression(p));
	}
	return e;
}

// done
static Expression *parseXorExpression(Parser* p){
	Expression *e = parseAndExpression(p)	;
	while(match(p, CARET)){
		e = createBinaryExpression(e, CARET, parseAndExpression(p));
	}
	return e;
}

// done
static Expression *parseOrExpression(Parser *p){
	Expression *e = parseXorExpression(p);
	while(match(p, PIPE)){
		e = createBinaryExpression(e, PIPE, parseXorExpression(p));
	}
	return e;
}

// done
static Expression *parseLogicalAndExpression(Parser *p) {
	Expression *e = parseOrExpression(p);
	if (match(p, AND_AND)) {
		return createBinaryExpression(e, AND_AND, parseOrExpression(p));
	}
	return e;
}

// done
static Expression *parseLogicalOrExpression(Parser *p) {
	Expression *e = parseLogicalAndExpression(p);
	if (match(p, PIPE_PIPE)) {
		return createBinaryExpression(e, PIPE_PIPE, parseLogicalAndExpression(p));
	}
	return e;
}

// done
static Expression *parseConditionalExpression(Parser *p) {
	Expression *e = parseLogicalOrExpression(p);
	if (match(p, QUESTION)) {
		Expression *ifTrue = parseExpression(p);
		consume(p, COLON);
		Expression *ifFalse = parseExpression(p);
		return createConditionalExpression(e, ifTrue, ifFalse);
	}
	return e;
}


// TODO: complete
static Expression *parseAssignmentExpression(Parser *p) {
	Expression *e = parseConditionalExpression(p);
	if (match(p, EQUAL) || match(p, PLUS_EQUAL) || match(p, MINUS_EQUAL) || match(p, STAR_EQUAL) || match(p, SLASH_EQUAL) || match(p, PERCENT_EQUAL)) {
		return createBinaryExpression(e, previous(p).type, parseAssignmentExpression(p));
	}
	return e;
}

// done
static Expression *parseCommaExpression(Parser *p) {
	Expression *e = parseAssignmentExpression(p);
	while (match(p, COMMA)) {
		e = createBinaryExpression(e, COMMA, parseAssignmentExpression(p));
	}
	return e;
	
}

// done
static Expression *parseExpression(Parser *p) {
  return parseCommaExpression(p);
}

static Statement *parseStatement(Parser *p) {
  Statement *s = createEmptyStatement();

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

  StatementList sl;
  sl.capacity = 10;
  sl.size = 0;
  sl.statements = malloc(sl.capacity * sizeof(Statement *));

  while (!match(p, RIGHT_BRACE)) {
    Statement *s = parseStatement(p);
    addStatement(&sl, s);
  }
	Statement *s = createFunctionDeclaration(name.lexeme, NULL, 0, sl.statements, sl.size, "int");
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
