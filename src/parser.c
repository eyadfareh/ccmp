#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
static Type *parseType(Parser *p);
static Statement *parseStatement(Parser *p);
static Expression *parseAssignmentExpression(Parser *p);
static Statement *parseDeclarationOrStatement(Parser *p);
static Statement *parseDeclaration(Parser *p);
static Statement *parseStatement(Parser *p);

// TODO: complete
static Expression *parsePrimaryExpression(Parser *p) {
  Expression *e = createEmptyExpression();
  if (match(p, LITERAL)) {
    e->type = LITERAL_EXPRESSION;
    e->as.literalExpression.value = atoi(previous(p).lexeme);
  } else if (match(p, IDENTIFIER)) {
    e->type = IDENTIFIER_EXPRESSION;
    e->as.identifierExpression.name = previous(p).lexeme;
  } else if (match(p, LEFT_PAREN)) {
    e = parseExpression(p);
    match(p, RIGHT_PAREN);
  } else if (match(p, STRING_LITERAL)) {
    e->type = STRING_EXPRESSION;
    e->as.stringExpression.value = malloc(strlen(previous(p).lexeme) + 1);
    strcpy(e->as.stringExpression.value, previous(p).lexeme);
  } else {
    free(e);
    return NULL;
  }
  return e;
}

static Expression *finishCall(Parser *p, Expression *e) {
  Expression *expr = createCallExpression(e, NULL, 0);
  if (match(p, LEFT_PAREN)) {
    return expr;
  }

  ExpressionList l = createEmptyExpressionList(10);
  while (!match(p, RIGHT_PAREN)) {
    addExpression(&l, parseAssignmentExpression(p));
    match(p, COMMA);
  }
  expr->as.callExpression.parameters = l.expressions;
  expr->as.callExpression.parametersCount = l.size;
  return expr;
}
// TODO: complete
static Expression *parsePostfixExpression(Parser *p) {
  Expression *expr = parsePrimaryExpression(p);

  while (match(p, LEFT_PAREN)) {
    switch (previous(p).type) {
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
  if (match(p, PLUS_PLUS) || match(p, MINUS_MINUS) || match(p, MINUS) ||
      match(p, TILDE) || match(p, BANG) || match(p, PLUS) || match(p, STAR) ||
      match(p, AND)) {
    TokenType op = previous(p).type;
    return createUnaryExpression(op, parseUnaryExpression(p));
  }
  return parsePostfixExpression(p);
}
// TODO: complete
static Expression *parseCastExpression(Parser *p) {
  return parseUnaryExpression(p);
}

// done
static Expression *parseMultiplicativeExpression(Parser *p) {
  Expression *e = parseCastExpression(p);
  while (match(p, STAR) || match(p, SLASH) || match(p, PERCENT)) {
    TokenType op = previous(p).type;
    e = createBinaryExpression(e, op, parseCastExpression(p));
  }
  return e;
}
// done
static Expression *parseAdditiveExpression(Parser *p) {
  Expression *e = parseMultiplicativeExpression(p);
  while (match(p, PLUS) || match(p, MINUS)) {
    TokenType op = previous(p).type;
    e = createBinaryExpression(e, op, parseMultiplicativeExpression(p));
  }
  return e;
}

// TODO: complete
static Expression *parseShiftExpression(Parser *p) {
  Expression *e = parseAdditiveExpression(p);
  return e;
}

// done
static Expression *parseRelationalExpression(Parser *p) {
  Expression *e = parseShiftExpression(p);
  while (match(p, LESS_THAN) || match(p, GREATER_THAN) ||
         match(p, LESS_EQUAL) || match(p, GREATER_EQUAL)) {
    TokenType op = previous(p).type;
    e = createBinaryExpression(e, op, parseShiftExpression(p));
  }
  return e;
}

// done
static Expression *parseEqualityExpression(Parser *p) {
  Expression *e = parseRelationalExpression(p);
  while (match(p, BANG_EQUAL) || match(p, EQUAL_EQUAL)) {

    TokenType op = previous(p).type;
    e = createBinaryExpression(e, op, parseRelationalExpression(p));
  }
  return e;
}

// done
static Expression *parseAndExpression(Parser *p) {
  Expression *e = parseEqualityExpression(p);
  while (match(p, AND)) {
    e = createBinaryExpression(e, AND, parseEqualityExpression(p));
  }
  return e;
}

// done
static Expression *parseXorExpression(Parser *p) {
  Expression *e = parseAndExpression(p);
  while (match(p, CARET)) {
    e = createBinaryExpression(e, CARET, parseAndExpression(p));
  }
  return e;
}

// done
static Expression *parseOrExpression(Parser *p) {
  Expression *e = parseXorExpression(p);
  while (match(p, PIPE)) {
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
  if (match(p, EQUAL) || match(p, PLUS_EQUAL) || match(p, MINUS_EQUAL) ||
      match(p, STAR_EQUAL) || match(p, SLASH_EQUAL) ||
      match(p, PERCENT_EQUAL)) {

    TokenType op = previous(p).type;
    return createBinaryExpression(e, op, parseAssignmentExpression(p));
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

static Type *parseType(Parser *p) {
  if(match(p, KEYWORD_INT)) {
    return createBasicType("int");
  }else if(match(p, KEYWORD_VOID)) {
    return createBasicType("void");
  }
}

static Statement *parseCompoundStatement(Parser *p) {
  consume(p, LEFT_BRACE);
  StatementList statements = createEmptyStatementList(3);
  while (!match(p, RIGHT_BRACE)) {
    // TODO: could be a statement or a declaration
    addStatement(&statements, parseDeclarationOrStatement(p));
  }

  return createCompoundStatement(statements.size, statements.statements);
}
static Statement *parseExpressionStatement(Parser *p) {
  if (match(p, SEMICOLON)) {
    return createExpressionStatement(NULL);
  }
  Expression *e = parseExpression(p);
  consume(p, SEMICOLON);
  return createExpressionStatement(e);
}

static Statement *parseReturnStatement(Parser *p) {
  consume(p, KEYWORD_RETURN);
  Expression *e = parseExpression(p);
  consume(p, SEMICOLON);
  return createReturnStatement(e);
}

static Statement *parseIfStatement(Parser *p) {
  consume(p, KEYWORD_IF);
  consume(p, LEFT_PAREN);
  Expression *condition = parseExpression(p);
  consume(p, RIGHT_PAREN);
  Statement *thenBranch = parseStatement(p);
  if (match(p, KEYWORD_ELSE)) {
    Statement *elseBranch = parseStatement(p);
    return createIfStatement(condition, thenBranch, elseBranch);
  }
  return createIfStatement(condition, thenBranch, NULL);

}
static Statement *parseStatement(Parser *p) {
  // compound-statement
  if (check(p, LEFT_BRACE)) {
    return parseCompoundStatement(p);
  }
  // jump-statement
  if (check(p, KEYWORD_RETURN)) {
    return parseReturnStatement(p);
  }
  
  if(check(p, KEYWORD_IF)) {
    return parseIfStatement(p);
  }

  return parseExpressionStatement(p);

  return NULL;
}

static Statement *parseDeclarationOrStatement(Parser *p) {
  if (check(p, KEYWORD_LET)) {
    return parseDeclaration(p);
  }
  return parseStatement(p);
}
static Statement *parseDeclaration(Parser *p) {
  if (match(p, KEYWORD_LET)) {
    consume(p, IDENTIFIER);
    char *name = previous(p).lexeme;
    Type *t = createBasicType("int");
    Expression *e = NULL;
    if (match(p, COLON)) {
      t = parseType(p);
    }
    if (match(p, EQUAL)) {
      e = parseExpression(p);
    }
    consume(p, SEMICOLON);
    return createVariableDeclaration(name, t, e);
  }
}
static Statement *parseFunctionDefinition(Parser *p) {
  consume(p, KEYWORD_FUNC);
  Token name = consume(p, IDENTIFIER);
  TokenList ps;
  ps.size = 0;
  ps.capacity = 3;
  ps.tokens = malloc(sizeof(Token) * ps.capacity);

  TypeList types = createEmptyTypeList(3);
  consume(p, LEFT_PAREN);

  while (!match(p, RIGHT_PAREN)) {
    addToken(&ps, consume(p, IDENTIFIER));
    if (match(p, COLON)) {
      addType(&types, parseType(p));
    } else {
      addType(&types, createBasicType("int"));
    }
    if (!match(p, COMMA)) {
      consume(p, RIGHT_PAREN);
      break;
    }
  }

  char **parameters = malloc(sizeof(char *) * (ps.size));

  for (int i = 0; i < ps.size; i++) {
    parameters[i] = ps.tokens[i].lexeme;
  }

  free(ps.tokens);
  Type *returnType = createBasicType("void");
  if (match(p, COLON)) {
    returnType = parseType(p);
  }

  Statement *body = parseCompoundStatement(p);
  return createFunctionDefinition(name.lexeme, ps.size, parameters, types.size, types.types,
                                  body, returnType);
}
static Statement *parsetTranslationUnit(Parser *p) {
  if (check(p, KEYWORD_FUNC)) {
    return parseFunctionDefinition(p);
  }
}

StatementList parse(Parser *p) {
  StatementList ls = createEmptyStatementList(10);
  while (!check(p, END_OF_FILE))
    addStatement(&ls, parsetTranslationUnit(p));
  return ls;
}
