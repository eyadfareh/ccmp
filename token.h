#pragma once


typedef enum 
{
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
	char* lexeme;
} Token;

