#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL) {
    perror("fopen");
    exit(1);
  }

  // get size
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  rewind(fp);

  // read
  char *buf = malloc(size + 1);
  if (buf == NULL) {
    perror("malloc");
    exit(1);
  }
  fread(buf, size, 1, fp);
  fclose(fp);

  buf[size] = '\0';
  printf("%s\n", buf);

  printf("=============================\n");
  Lexer lexer = createLexer(buf, size);
  TokenList tokens = tokenize(&lexer);
  for (int i = 0; i < tokens.size; i++) {
    printf("%d %d %d '%s'\n", tokens.tokens[i].line, tokens.tokens[i].col,
           tokens.tokens[i].type, tokens.tokens[i].lexeme);
  }

  free(buf);
  free(tokens.tokens);
  return 0;
}
