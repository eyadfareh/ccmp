#include "parser.h"
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
