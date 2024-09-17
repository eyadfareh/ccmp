#include "ast.h"
#include "code_generator.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "qbe.h"
int compile(char *filename) {
  FILE *fp = fopen(filename, "r");
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
  printf("=============================\n");
  Parser p = createParser(tokens);
  StatementList statements = parse(&p);
  for (int i = 0; i < statements.size; i++) {
    printStatement(statements.statements[i], 0);
  }
  printf("=============================\n");


  FILE *f = fopen("temp.c", "w");
  Program prog = create_program(&statements, f);
  generate(&prog);
  fclose(f);

  system("gcc temp.c -o temp");
  system("rm -rf temp.c");

  for (int i = 0; i < statements.size; i++)
    freeStatement(statements.statements[i]);
  //free(q);
  free(statements.statements);
  free(lexer.lexemeBuffer);
  free(buf);
  free(tokens.tokens);
  return 0;
}
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  compile(argv[1]);
  return 0;
}
