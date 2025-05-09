#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "string_builder.h"
#include "string_view.h"

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
    "let", "if", "return", "use",
    "not", "and", "or"
};

bool is_keyword(const char *str, size_t len) {
    StringView chunk = sv_from_parts(str, len);
    for (int i=0; i<sizeof(keywords)/sizeof(*keywords); i++) {
        if (sv_equals_cstr(chunk, keywords[i]))
            return true;
    }
    return false;
}

char *src;
char *current;

// get character at current ptr
char peek() {
    return *current;
}

// get current char, then increment by 1
char advance() {
    return *current++;
}

void lexer_init(const char* input) {
    src = (char*)input;
    current = src;
}

Token token_next(Arena *a) {
    while(is_space(peek())) current++; // skip leading whitespace

    const char* start = current;

    if (peek() == '\0')
        return (Token) { TOKEN_EOF };

    char c = advance(); 

    // single-character tokens
    switch (c) {
        case '+' : return (Token) { TOKEN_PLUS,      (char*)start, 1 };
        case '-' : return (Token) { TOKEN_MINUS,     (char*)start, 1 };
        case '*' : return (Token) { TOKEN_STAR,      (char*)start, 1 };
        case '/' : return (Token) { TOKEN_SLASH,     (char*)start, 1 };
        case '<' : return (Token) { TOKEN_LESS,    (char*)start, 1 };
        case '>' : return (Token) { TOKEN_GREATER,    (char*)start, 1 };
        case '=' : return (Token) { TOKEN_EQUALS,    (char*)start, 1 };
        case ',' : return (Token) { TOKEN_COMMA,    (char*)start, 1 };
        case ';' : return (Token) { TOKEN_SEMICOLON, (char*)start, 1 };
        case '(' : return (Token) { TOKEN_LPAREN,    (char*)start, 1 };
        case ')' : return (Token) { TOKEN_RPAREN,    (char*)start, 1 };
        case '{' : return (Token) { TOKEN_LBRACE,    (char*)start, 1 };
        case '}' : return (Token) { TOKEN_RBRACE,    (char*)start, 1 };
    }

    // string literal
    if (c == '"') {
        StringBuilder *temp = sb_new();

        while (true) {
            char ch = advance();

            if (ch == '\0') return (Token) { TOKEN_UNKNOWN, (char*)start, current - start };
            if (ch == '"')  break;

            if (ch == '\\') {
                ch = advance();
                switch (ch) {
                    case 'n':
                        sb_append_char(temp, '\n');
                        break;
                    case '\\':
                        sb_append_char(temp, '\\');
                        break;
                    case '"':
                        sb_append_char(temp, '"');
                        break;
                    default:
                        sb_append_char(temp, ch); // unknown escape character, keep as is lol
                        break;
                }
            }
            else {
                sb_append_char(temp, ch);
            }
        }

        size_t len = temp->len;
        char *str = arena_alloc(a, len);
        memcpy(str, temp->data, len);
        sb_free(temp);

        return (Token) { TOKEN_STRING, sv_from_parts(str, len) };
    }

    // keywords / identifiers
    if (is_alpha(c)) {
        while(is_alpha(peek()) || is_digit(peek())) advance();
        int len = current - start;
        enum TokenType type = is_keyword(start, len) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        return (Token) { type, (char*)start, len };
    }

    // Numeros
    if (is_digit(c)) {
        while(is_digit(peek())) advance();
        return (Token) { TOKEN_NUMERIC, (char*)start, current - start };
    }

    // Else, unknown token
    return (Token) { TOKEN_UNKNOWN, (char*)start, 1 };
}

void token_print(Token t) {
    char *typeStr = "";
    switch(t.type) {
        case TOKEN_KEYWORD: typeStr = "TOKEN_KEYWORD"; break;
        case TOKEN_IDENTIFIER: typeStr = "TOKEN_IDENTIFIER"; break;
        case TOKEN_NUMERIC: typeStr = "TOKEN_NUMERIC"; break;
        case TOKEN_PLUS: typeStr = "TOKEN_PLUS"; break;
        case TOKEN_MINUS: typeStr = "TOKEN_MINUS"; break;
        case TOKEN_STAR: typeStr = "TOKEN_STAR"; break;
        case TOKEN_SLASH: typeStr = "TOKEN_SLASH"; break;
        case TOKEN_LESS: typeStr = "TOKEN_LESS"; break;
        case TOKEN_GREATER: typeStr = "TOKEN_GREATER"; break;
        case TOKEN_EQUALS: typeStr = "TOKEN_EQUALS"; break;
        case TOKEN_LPAREN: typeStr = "TOKEN_LPAREN"; break;
        case TOKEN_RPAREN: typeStr = "TOKEN_RPAREN"; break;
        case TOKEN_LBRACE: typeStr = "TOKEN_LBRACE"; break;
        case TOKEN_RBRACE: typeStr = "TOKEN_RBRACE"; break;
        case TOKEN_COMMA: typeStr = "TOKEN_COMMA"; break;
        case TOKEN_SEMICOLON: typeStr = "TOKEN_SEMICOLON"; break;
        case TOKEN_STRING: typeStr = "TOKEN_STRING"; break;
        case TOKEN_EOF: typeStr = "TOKEN_EOF"; break;
        case TOKEN_UNKNOWN: typeStr = "TOKEN_UNKNOWN"; break;
    }

    printf("%s: "SV_Fmt"\n", typeStr, SV_Arg(t.content));
}
