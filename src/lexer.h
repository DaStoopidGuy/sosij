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
    TOKEN_EQUALS,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURLY,
    TOKEN_RCURLY,
    TOKEN_SEMICOLON,
    TOKEN_EOF, // End Of File, Khatm. Tata. Bye-bye.
    TOKEN_UNKNOWN,
};

struct Token {
    enum TokenType type;
    char *data;
    size_t len;
};

void lexer_init(const char* input);
struct Token token_next();
void token_print(struct Token t);
