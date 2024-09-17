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
static bool isNumeric(char x) { return (x >= '0' && x <= '9'); }
static char peek(Lexer *l) { return l->buffer[l->current]; }
Lexer createLexer(char *buffer, int length) {
  Lexer lexer;
  lexer.buffer = buffer;
  lexer.lexemeBuffer = (char *)malloc(length * 2);
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
  strncpy(token.lexeme, lexer->buffer + lexer->start,
          lexer->current - lexer->start);
  lexer->lexemePos += lexer->current - lexer->start;
  lexer->lexemeBuffer[lexer->lexemePos] = '\0';
  lexer->lexemePos++;

  lexer->col += lexer->current - lexer->start;
  lexer->start = lexer->current;
  return token;
}
static Token getIdentifier(Lexer *lexer) {
  Token t = getToken(lexer, IDENTIFIER);
  if (strcmp(t.lexeme, "int") == 0) {
    t.type = KEYWORD_INT;
  } else if (strcmp(t.lexeme, "return") == 0) {
    t.type = KEYWORD_RETURN;
  } else if (strcmp(t.lexeme, "float") == 0) {
    t.type = KEYWORD_FLOAT;
  } else if (strcmp(t.lexeme, "double") == 0) {
    t.type = KEYWORD_DOUBLE;
  } else if (strcmp(t.lexeme, "void") == 0) {
    t.type = KEYWORD_VOID;
  } else if (strcmp(t.lexeme, "let") == 0) {
    t.type = KEYWORD_LET;
  } else if (strcmp(t.lexeme, "func") == 0) {
    t.type = KEYWORD_FUNC;
  } else if (strcmp(t.lexeme, "extern") == 0) {
    t.type = KEYWORD_EXTERN;
  } else if (strcmp(t.lexeme, "if") == 0) {
    t.type = KEYWORD_IF;
  } else if (strcmp(t.lexeme, "else") == 0) {
    t.type = KEYWORD_ELSE;
  } else if (strcmp(t.lexeme, "for") == 0) {
    t.type = KEYWORD_FOR;
  } else if (strcmp(t.lexeme, "while") == 0) {
    t.type = KEYWORD_WHILE;
  } else if (strcmp(t.lexeme, "true") == 0) {
    t.type = KEYWORD_TRUE;
  } else if (strcmp(t.lexeme, "false") == 0) {
    t.type = KEYWORD_FALSE;
  }
  return t;
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

Token nextToken(Lexer *l) {
  char c = peek(l);
  switch (c) {
  case '\0':
    return getToken(l, END_OF_FILE);
  case '\n':
    l->line++;
    l->col = 0;
  case ' ':
  case '\t':
  case '\r':
    l->current++;
    l->col++;
    l->start = l->current;
    return nextToken(l);
  // Punctuators
  case '[':
    l->current++;
    return getToken(l, LEFT_BRACKET);
  case ']':
    l->current++;
    return getToken(l, RIGHT_BRACKET);
  case '(':
    l->current++;
    return getToken(l, LEFT_PAREN);
  case ')':
    l->current++;
    return getToken(l, RIGHT_PAREN);
  case '{':
    l->current++;
    return getToken(l, LEFT_BRACE);
  case '}':
    l->current++;
    return getToken(l, RIGHT_BRACE);
  case '.':
    l->current++;
    if (peek(l) == '.') {
      l->current++;
      if (peek(l) == '.') {
        l->current++;
        return getToken(l, DOT_DOT_DOT);
      }
      l->current--;
    }
    return getToken(l, DOT);
  case '-':
    l->current++;
    if (peek(l) == '>') {
      l->current++;
      return getToken(l, RIGHT_ARROW);
    } else if (peek(l) == '-') {
      l->current++;
      return getToken(l, MINUS_MINUS);
    } else if (peek(l) == '=') {
      l->current++;
      return getToken(l, MINUS_EQUAL);
    }
    return getToken(l, MINUS);
  case '+':
    l->current++;
    if (peek(l) == '+') {
      l->current++;
      return getToken(l, PLUS_PLUS);
    } else if (peek(l) == '=') {
      l->current++;
      return getToken(l, PLUS_EQUAL);
    }
    return getToken(l, PLUS);
  case '&':
    l->current++;
    if (peek(l) == '&') {
      l->current++;
      return getToken(l, AND_AND);
    } else if (peek(l) == '=') {
      l->current++;
      return getToken(l, AND_EQUAL);
    }
    return getToken(l, AND);
  case '*':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, STAR_EQUAL);
    }
    return getToken(l, STAR);
  case '~':
    l->current++;
    return getToken(l, TILDE);
  case '!':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, BANG_EQUAL);
    }
    return getToken(l, BANG);
  case '/':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, SLASH_EQUAL);
    }
    return getToken(l, SLASH);
  case '%':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, PERCENT_EQUAL);
    }
    return getToken(l, PERCENT);
  // TODO: << and >>
  case '<':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, LESS_EQUAL);
    }
    return getToken(l, LESS_THAN);
  case '>':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, GREATER_EQUAL);
    }
    return getToken(l, GREATER_THAN);
  case '=':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, EQUAL_EQUAL);
    }
    return getToken(l, EQUAL);
  case '^':
    l->current++;
    if (peek(l) == '=') {
      l->current++;
      return getToken(l, CARET_EQUAL);
    }
    return getToken(l, CARET);
  case '|':
    l->current++;
    if (peek(l) == '|') {
      l->current++;
      return getToken(l, PIPE_PIPE);
    } else if (peek(l) == '=') {
      l->current++;
      return getToken(l, OR_EQUAL);
    }
    return getToken(l, PIPE);
  case '?':
    l->current++;
    return getToken(l, QUESTION);

  case ':':
    l->current++;
    return getToken(l, COLON);
  case ';':
    l->current++;
    return getToken(l, SEMICOLON);
    // TODO: <<= and >>=
  case ',':
    l->current++;
    return getToken(l, COMMA);

  case '#':
    l->current++;
    if (peek(l) == '#') {
      l->current++;
      return getToken(l, HASH_HASH);
    }
    return getToken(l, HASH);
  case '"':
    l->current++;
    while (peek(l) != '"') {
      l->current++;
    }
    l->current++;
    return getToken(l, STRING_LITERAL);

  default:
    if (isAlpha(peek(l))) {

      while (isAlphaNumerical(peek(l))) {
        l->current++;
      }

      return getIdentifier(l);
    }
    if (isNumeric(peek(l))) {
      while (isNumeric(peek(l))) {
        l->current++;
      }
      return getToken(l, LITERAL);
    }
  }

  printf("WARNING: UNREACHABLE CODE REACHED; lexer.c::nextToken; \n");
  printf("lexer->current = %d; peek(lexer) = '%c'\n", l->current, peek(l));
  l->current++;
  return getToken(l, IDENTIFIER);
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
