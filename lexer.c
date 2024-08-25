#include "lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static bool isAlphaNumerical(char x) {
  return ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') ||
          (x >= '0' && x <= '9') || x == '_');
}
static bool isAlpha(char x) {
  return ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || x == '_');
}
static bool isNumeric(char x){
  return (x >= '0' && x <= '9');
}
static char peek(Lexer *l) { return l->buffer[l->current]; }
Lexer createLexer(char *buffer, int length) {
  Lexer lexer;
  lexer.buffer = buffer;
  lexer.lexemeBuffer = (char*) malloc(length*2);
  lexer.lexemePos = 0;
  lexer.line = 1;
  lexer.col = 1;
  lexer.start = 0;
  lexer.current = 0;
  lexer.tokens.tokens = malloc(sizeof(Token) * 100);
  lexer.length = length;
  if (lexer.tokens.tokens == NULL) {
    perror("malloc");
    exit(1);
  }
  lexer.tokens.size = 0;
  lexer.tokens.capacity = 100;
  return lexer;
}
Token getToken(Lexer *lexer, TokenType type) {
  Token token;
  token.type = type;
  token.line = lexer->line;
  token.col = lexer->col;
  // TODO: DO NOT ALLOCATE HERE
  token.lexeme = (lexer->lexemeBuffer + lexer->lexemePos);
  strncpy(token.lexeme, lexer->buffer + lexer->start, lexer->current - lexer->start);
  lexer->lexemePos += lexer->current - lexer->start;
  lexer->lexemeBuffer[lexer->lexemePos] = '\0';
  lexer->lexemePos++;

  lexer->col += lexer->current - lexer->start;
  lexer->start = lexer->current;
  return token;
}
void addToken(TokenList *tokenList, Token token) {
  if (tokenList->capacity == tokenList->size) {
    tokenList->capacity *= 2;
    tokenList->tokens =
        realloc(tokenList->tokens, sizeof(Token) * tokenList->capacity);
    if (tokenList->tokens == NULL) {
      perror("realloc");
      exit(1);
    }
  }

  tokenList->tokens[tokenList->size++] = token;
}

Token nextToken(Lexer *lexer) {
  char c = peek(lexer);
  switch (c) {
  case '\0':
    return getToken(lexer, END_OF_FILE);
  case '\n':
    lexer->line++;
    lexer->col = 0;
  case ' ':
  case '\t':
  case '\r':
    lexer->current++;
    lexer->col++;
    lexer->start = lexer->current;
    return nextToken(lexer);
  case '+':
    lexer->current++;
    return getToken(lexer, PLUS);
  case '-':
    lexer->current++;
    return getToken(lexer, MINUS);
  case '*':
    lexer->current++;
    return getToken(lexer, MULTIPLY);
  case '/':
    lexer->current++;
    return getToken(lexer, DIVIDE);
  case '(':
    lexer->current++;
    return getToken(lexer, LEFT_PAREN);
  case ')':
    lexer->current++;
    return getToken(lexer, RIGHT_PAREN);
  case '{':
    lexer->current++;
    return getToken(lexer, LEFT_BRACE);
  case '}':
    lexer->current++;
    return getToken(lexer, RIGHT_BRACE);
  case ';':
    lexer->current++;
    return getToken(lexer, SEMICOLON);
  case '=':
    lexer->current++;
    return getToken(lexer, EQUAL);
  case '"':
    lexer->current++;
    while (peek(lexer) != '"') {
      lexer->current++;
    }
    lexer->current++;
    return getToken(lexer, STRING_LITERAL);

  default:
    if (isAlpha(peek(lexer))) {
      while (isAlphaNumerical(peek(lexer))) {
        lexer->current++;
      }

      return getToken(lexer, IDENTIFIER);
    }
    if(isNumeric(peek(lexer))){
      while(isNumeric(peek(lexer))){
        lexer->current++;
      }
      return getToken(lexer, LITERAL);
    }
  }

  printf("WARNING: UNREACHABLE CODE REACHED; lexer.c::nextToken; \n");
  printf("lexer->current = %d; peek(lexer) = '%c'\n", lexer->current,
         peek(lexer));
  lexer->current++;
  return getToken(lexer, IDENTIFIER);
}
TokenList tokenize(Lexer *lexer) {
  while (lexer->current < lexer->length) {
    Token t = nextToken(lexer);
    addToken(&lexer->tokens, t);
    if (t.type == END_OF_FILE) {
      break;
    }
  }
  return lexer->tokens;
}
