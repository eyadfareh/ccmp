#pragma once
#include "lexer.h"

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
      Token functionName;
      int argc;
      char **argv; // lol
      int statementsLength;
      ASTNode** statements;
    } functionDeclaration;
    struct {
      ASTNode* val;
    } returnStatement ;
    struct {
      Token functionName;
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
} Parser;

ASTNode* createEmptyNode();
void freeNode(ASTNode* n);
