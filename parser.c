#include "parser.h"
#include "lexer.h"
#include <stddef.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

ASTNode *createEmptyNode() {
  ASTNode *nn = (ASTNode *)malloc(sizeof(ASTNode));
  return nn;
}

void freeNode(ASTNode *n) {
  switch (n->type) {
  case FUNCTION_CALL_STATEMENT:
    free(n->as.functionCall.argv);
    break;
  case FUNCTION_DECLARATION:
    for (int i = 0; i < n->as.functionDeclaration.statementsLength; i++) {
      freeNode(n->as.functionDeclaration.statements[i]);
    }
    free(n->as.functionDeclaration.argv);
    break;
  case RETURN_STATEMENT:
    freeNode(n->as.returnStatement.val);
    break;
  }

  free(n);
}


Parser createParser(TokenList tokens){
  Parser p;
  p.tokens = tokens;
  p.current = 0;
  return p;
}

static Token peek(Parser* p){
  return p->tokens.tokens[p->current];
}
static Token previous(Parser *p){
  return p->tokens.tokens[p->current-1];
}
static bool isAtEnd(Parser *p){
  return peek(p).type == END_OF_FILE;
}
static bool check(Parser *p,TokenType t){
  if(isAtEnd(p)) return false;
  return peek(p).type == t;
}
static Token advance(Parser* p){
  if(!isAtEnd(p)) p->current++;
  return previous(p);
}
static bool match(Parser *p, TokenType t){
  if(check(p, t)){
    advance(p);
    return true;
  }
  return false;
}

static Token consume(Parser* p, TokenType type) {
  if (check(p, type)) return advance(p);
}

static ASTNode* parseType(Parser* p){
  ASTNode* n = createEmptyNode();
  n->type = TYPE_EXPRESSION;
  if(match(p, KEYWORD_INT)){
    n->as.typeExpression.type = INT_TYPE;
  }else{
  }
  return n;
}
static ASTNode* parseExpression(Parser* p){
  ASTNode* n = createEmptyNode();
  if(match(p, LITERAL)){
    n->type = PRIMARY_EXPRESSION;
    n->as.primaryExpression.x = 0;
  }
  return NULL;
}
static ASTNode* parseStatement(Parser* p){
  ASTNode* n = createEmptyNode();
  if(match(p, KEYWORD_RETURN)){
    ASTNode* expr = parseExpression(p);
    consume(p, SEMICOLON);
    n->type = RETURN_STATEMENT;
    n->as.returnStatement.val = expr;
  }

  return NULL;
}
static ASTNode* parseDeclaration(Parser* p){
  ASTNode* type = parseType(p);
  Token function_name = consume(p, IDENTIFIER);
  consume(p, LEFT_PAREN);
  consume(p, RIGHT_PAREN);
  consume(p, LEFT_BRACE);
  while(!check(p, RIGHT_BRACE)){
    ASTNode* stmt = parseStatement(p);
  }
  consume(p, RIGHT_BRACE);




  ASTNode* n = createEmptyNode();
  n->type = FUNCTION_DECLARATION;
  n->as.functionDeclaration.functionName = function_name.lexeme;
  n->as.functionDeclaration.argc = 0;
  n->as.functionDeclaration.argv = NULL;
  n->as.functionDeclaration.statementsLength = 0;
  n->as.functionDeclaration.statements= NULL;
  return n;
}

ASTNode* parse(Parser *p){
  return parseDeclaration(p);
}
