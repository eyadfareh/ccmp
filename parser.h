#pragma once


typedef enum TYPES {
  INT_TYPE
};

typedef enum{
  FUNCTION_DECLARATION,
  TYPE_EXPRESSION,
} ASTType;

typedef struct ASTNode ASTNode;
struct ASTNode{
  ASTType type;
  union {
  } as;
};
