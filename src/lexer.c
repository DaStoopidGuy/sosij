#include <stdbool.h>
#include <string.h>
#include "lexer.h"

#include <stdio.h>

/*
 * What does Lexer do?
 * ----
 * - takes input: source code
 * - splits into Tokens by reading the text
 * - appends said tokens into a list
 */

bool is_space(char c) {
    return c==' ' || c=='\n' || c=='\t' || c=='\r';
}

bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

const char *keywords[] = {
    "let", "if"
};

bool is_keyword(const char *str, int len) {
    for (int i=0; i<sizeof(keywords)/sizeof(*keywords); i++) {
        if (strncmp(str, keywords[i], len) == 0 && keywords[i][len] == '\0')
            return true;
    }
    return false;
}

char *src;
char *current;

void lexer_init(const char* input) {
    src = (char*)input;
    current = src;
}

struct Token token_next() {
    while(is_space(*current)) current++; // skip leading whitespace

    const char* start = current;

    if (*current == '\0')
        return (struct Token) { TOKEN_EOF };

    char c = *current++; // get current char, then increment

    // single-character tokens
    switch (c) {
        case '+' : return (struct Token) { TOKEN_PLUS,      (char*)start, 1 };
        case '-' : return (struct Token) { TOKEN_MINUS,     (char*)start, 1 };
        case '*' : return (struct Token) { TOKEN_STAR,      (char*)start, 1 };
        case '/' : return (struct Token) { TOKEN_SLASH,     (char*)start, 1 };
        case '=' : return (struct Token) { TOKEN_EQUALS,    (char*)start, 1 };
        case ';' : return (struct Token) { TOKEN_SEMICOLON, (char*)start, 1 };
        case '(' : return (struct Token) { TOKEN_LPAREN,    (char*)start, 1 };
        case ')' : return (struct Token) { TOKEN_RPAREN,    (char*)start, 1 };
        case '{' : return (struct Token) { TOKEN_LCURLY,    (char*)start, 1 };
        case '}' : return (struct Token) { TOKEN_RCURLY,    (char*)start, 1 };
    }

    // keywords / identifiers
    if (is_alpha(c)) {
        while(is_alpha(*current) || is_digit(*current)) current++;
        int len = current - start;
        enum TokenType type = is_keyword(start, len) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        return (struct Token) { type, (char*)start, len };
    }

    // Numeros
    if (is_digit(c)) { // TODO: floating point nums not supported
        while(is_digit(*current)) current++;
        return (struct Token) { TOKEN_NUMERIC, (char*)start, current - start };
    }

    // Else, unknown token
    return (struct Token) { TOKEN_UNKNOWN, (char*)start, 1 };
}

void token_print(struct Token t) {
    char *typeStr = "";
    switch(t.type) {
        case TOKEN_KEYWORD: typeStr = "TOKEN_KEYWORD"; break;
        case TOKEN_IDENTIFIER: typeStr = "TOKEN_IDENTIFIER"; break;
        case TOKEN_NUMERIC: typeStr = "TOKEN_NUMERIC"; break;
        case TOKEN_PLUS: typeStr = "TOKEN_PLUS"; break;
        case TOKEN_MINUS: typeStr = "TOKEN_MINUS"; break;
        case TOKEN_STAR: typeStr = "TOKEN_STAR"; break;
        case TOKEN_SLASH: typeStr = "TOKEN_SLASH"; break;
        case TOKEN_EQUALS: typeStr = "TOKEN_EQUALS"; break;
        case TOKEN_LPAREN: typeStr = "TOKEN_LPAREN"; break;
        case TOKEN_RPAREN: typeStr = "TOKEN_RPAREN"; break;
        case TOKEN_LCURLY: typeStr = "TOKEN_LCURLY"; break;
        case TOKEN_RCURLY: typeStr = "TOKEN_RCURLY"; break;
        case TOKEN_SEMICOLON: typeStr = "TOKEN_SEMICOLON"; break;
        case TOKEN_EOF: typeStr = "TOKEN_EOF"; break;
        case TOKEN_UNKNOWN: typeStr = "TOKEN_UNKNOWN"; break;
    }

    printf("%s: %.*s\n", typeStr, (int)t.len, t.data);
}
