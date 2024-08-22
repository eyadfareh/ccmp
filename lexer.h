#pragma once

typedef enum {
  IDENTIFIER,
  INTEGER,
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BRACKET,
  RIGHT_BRACKET,
  SEMICOLON,
  COMMA,
  EQUAL,
  END_OF_FILE
} TokenType;

typedef struct {
  TokenType type;
  int line;
  int col;
  int start;
  int end;
} Token;

typedef struct {
  Token *tokens;
  int size;
  int capacity;
} TokenList;

typedef struct {
  char *buffer;
  int line;
  int col;
  int start;
  int current;
  int length;
  TokenList tokens;
} Lexer;

Lexer createLexer(char *buffer, int length);
Token getToken(Lexer *lexer, TokenType type);
void addToken(TokenList *list, Token token);
Token nextToken(Lexer *lexer);
TokenList tokenize(Lexer *lexer);
