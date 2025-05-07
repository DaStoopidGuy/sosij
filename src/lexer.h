#pragma once
#include <stdlib.h>

enum TokenType {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMERIC,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LESS, // <
    TOKEN_GREATER, // >
    TOKEN_EQUALS,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_STRING, // string literal ("This is a string")
    TOKEN_EOF, // End Of File, Khatm. Tata. Bye-bye.
    TOKEN_UNKNOWN,
};

typedef struct {
    enum TokenType type;
    char *data;
    size_t len;
} Token;

void lexer_init(const char* input);
Token token_next();
void token_print(Token t);
