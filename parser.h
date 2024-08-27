#pragma once
#include "lexer.h"
#include <wctype.h>

typedef enum {
  INT_TYPE
} Type;

typedef enum{
  FUNCTION_DECLARATION,
  RETURN_STATEMENT,
  FUNCTION_CALL_STATEMENT,
  TYPE_EXPRESSION,
  PRIMARY_EXPRESSION,
} ASTType;

typedef struct ASTNode ASTNode;
struct ASTNode{
  ASTType type;
  union {
    struct {
      char* functionName;
      int argc;
      char **argv; // lol
      int statementsLength;
      ASTNode** statements;
    } functionDeclaration;
    struct {
      ASTNode* val;
    } returnStatement ;
    struct {
      char* functionName;
      int argc;
      char **argv; // lol
    } functionCall;
    struct {
      Type type;
    } typeExpression;
    struct {
      int x;
    } primaryExpression;
  } as;
};

typedef struct {
  TokenList tokens;
  int current;
} Parser;

ASTNode* createEmptyNode();
void freeNode(ASTNode* n);
Parser createParser(TokenList);
ASTNode* parse(Parser* p);
