#pragma once

typedef enum {
  IDENTIFIER,
  LITERAL,
  STRING_LITERAL,

 // Punctuators
	LEFT_BRACKET,
	RIGHT_BRACKET,
	LEFT_PAREN,
	RIGHT_PAREN,
	LEFT_BRACE,
	RIGHT_BRACE,
	DOT,
	RIGHT_ARROW,
	PLUS_PLUS,
	MINUS_MINUS,
	AND,
	STAR,
	PLUS,
	MINUS,
	TILDE,
	BANG,
	SLASH,
	PERCENT,
	// TODO: << and >>
	LESS_THAN,
	GREATER_THAN,
	LESS_EQUAL,
	GREATER_EQUAL,
	EQUAL_EQUAL,
	BANG_EQUAL,
	CARET,
	PIPE,
	AND_AND,
	PIPE_PIPE,
	QUESTION,
	COLON,
	SEMICOLON,
	DOT_DOT_DOT,
	EQUAL,
	STAR_EQUAL,
	SLASH_EQUAL,
	PERCENT_EQUAL,
	PLUS_EQUAL,
	MINUS_EQUAL,
	// TODO: <<= and >>=
	AND_EQUAL,
	CARET_EQUAL,
	OR_EQUAL,
	COMMA,
	HASH,
	HASH_HASH,
	// TODO: <:  >: <% %> %: and %:%:
	
	// Keywords
	KEYWORD_AUTO,
	KEYWORD_BREAK,
	KEYWORD_CASE,
	KEYWORD_CHAR,
	KEYWORD_CONST,
	KEYWORD_CONTINUE,
	KEYWORD_DEFAULT,
	KEYWORD_DO,
	KEYWORD_DOUBLE,
	KEYWORD_ELSE,
	KEYWORD_ENUM,
	KEYWORD_EXTERN,
	KEYWORD_FLOAT,
	KEYWORD_FOR,
	KEYWORD_GOTO,
	KEYWORD_IF,
	KEYWORD_INLINE,
  KEYWORD_INT,
	KEYWORD_LONG,
	KEYWORD_REGISTER,
	KEYWORD_RESTRICT,
  KEYWORD_RETURN,
	KEYWORD_SHORT,
	KEYWORD_SIGNED,
	KEYWORD_SIZEOF,
	KEYWORD_STATIC,
	KEYWORD_STRUCT,
	KEYWORD_SWITCH,
	KEYWORD_TYPEDEF,
	KEYWORD_UNION,
	KEYWORD_UNSIGNED,
	KEYWORD_VOID,
	KEYWORD_VOLATILE,
	KEYWORD_WHILE,
	KEYWORD_LET,
	KEYWORD_FUNC,

  END_OF_FILE
} TokenType;

typedef struct {
  TokenType type;
  int line;
  int col;
  char* lexeme;
} Token;

typedef struct {
  Token *tokens;
  int size;
  int capacity;
} TokenList;

typedef struct {
  char *buffer;
  char *lexemeBuffer;
  int lexemePos;
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
