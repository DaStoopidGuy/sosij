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
    return c==' ' || c=='\t' || c=='\r';
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
    for (size_t i=0; i<sizeof(keywords)/sizeof(*keywords); i++) {
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
        return (Token) { TOKEN_EOF, sv_from_parts(NULL, 0) };

    char c = advance(); 

    // single-character tokens
    switch (c) {
        case '+' : return (Token) { TOKEN_PLUS,      sv_from_parts(start, 1) };
        case '-' : return (Token) { TOKEN_MINUS,     sv_from_parts(start, 1) };
        case '*' : return (Token) { TOKEN_STAR,      sv_from_parts(start, 1) };
        case '/' : return (Token) { TOKEN_SLASH,     sv_from_parts(start, 1) };
        case ',' : return (Token) { TOKEN_COMMA,     sv_from_parts(start, 1) };
        case '\n': return (Token) { TOKEN_NEWLINE,   sv_from_parts(start, 1) };
        case '(' : return (Token) { TOKEN_LPAREN,    sv_from_parts(start, 1) };
        case ')' : return (Token) { TOKEN_RPAREN,    sv_from_parts(start, 1) };
        case '{' : return (Token) { TOKEN_LBRACE,    sv_from_parts(start, 1) };
        case '}' : return (Token) { TOKEN_RBRACE,    sv_from_parts(start, 1) };
    }

    // = assignment and == equality operators
    if (c == '=') {
        if (peek() == '=') {
            advance();
            return (Token) { TOKEN_EQ, sv_from_parts(start, 2) };
        }
        else
            return (Token) { TOKEN_ASSIGN, sv_from_parts(start, 1) };
    }

    // > greater than and greater than or equals operator
    if (c == '>') {
        if (peek() == '=') {
            advance();
            return (Token) { TOKEN_GTEQ, sv_from_parts(start, 2) };
        }
        else
            return (Token) { TOKEN_GT, sv_from_parts(start, 1) };
    }

    // < less than and <= less than or equals operators
    if (c == '<') {
        if (peek() == '=') {
            advance();
            return (Token) { TOKEN_LTEQ, sv_from_parts(start, 2) };
        }
        else
            return (Token) { TOKEN_LT, sv_from_parts(start, 1) };
    }

    // ! not and != not equals operators
    if (c == '!') {
        if (peek() == '=') {
            advance();
            return (Token) { TOKEN_NOTEQ, sv_from_parts(start, 2) };
        }
        else
            return (Token) { TOKEN_NOT, sv_from_parts(start, 1) };
    }

    // && and operator
    if (c == '&') {
        if (peek() == '&') {
            advance();
            return (Token) { TOKEN_AND, sv_from_parts(start, 2) };
        }
        else
            return (Token) { TOKEN_UNKNOWN, sv_from_parts(start, 1) };
    }

    // || or operator
    if (c == '|') {
        if (peek() == '|') {
            advance();
            return (Token) { TOKEN_OR, sv_from_parts(start, 2) };
        }
        else
            return (Token) { TOKEN_UNKNOWN, sv_from_parts(start, 1) };
    }

    // string literal
    if (c == '"') {
        StringBuilder *temp = sb_new();

        while (true) {
            char ch = advance();

            if (ch == '\0') return (Token) { TOKEN_UNKNOWN, sv_from_parts(start, current - start) };
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
        return (Token) { type, sv_from_parts(start, len) };
    }

    // Numeros
    if (is_digit(c)) {
        while(is_digit(peek())) advance();
        return (Token) { TOKEN_NUMERIC, sv_from_parts(start, current - start) };
    }

    // Else, unknown token
    return (Token) { TOKEN_UNKNOWN, sv_from_parts(start, 1) };
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
        case TOKEN_LT: typeStr = "TOKEN_LT"; break;
        case TOKEN_GT: typeStr = "TOKEN_GT"; break;
        case TOKEN_LTEQ: typeStr = "TOKEN_LTEQ"; break;
        case TOKEN_GTEQ: typeStr = "TOKEN_GTEQ"; break;
        case TOKEN_EQ: typeStr = "TOKEN_EQ"; break;
        case TOKEN_NOTEQ: typeStr = "TOKEN_NOTEQ"; break;
        case TOKEN_NOT: typeStr = "TOKEN_NOT"; break;
        case TOKEN_AND: typeStr = "TOKEN_AND"; break;
        case TOKEN_OR: typeStr = "TOKEN_OR"; break;
        case TOKEN_ASSIGN: typeStr = "TOKEN_ASSIGN"; break;
        case TOKEN_LPAREN: typeStr = "TOKEN_LPAREN"; break;
        case TOKEN_RPAREN: typeStr = "TOKEN_RPAREN"; break;
        case TOKEN_LBRACE: typeStr = "TOKEN_LBRACE"; break;
        case TOKEN_RBRACE: typeStr = "TOKEN_RBRACE"; break;
        case TOKEN_COMMA: typeStr = "TOKEN_COMMA"; break;
        case TOKEN_NEWLINE: typeStr = "TOKEN_NEWLINE"; break;
        case TOKEN_STRING: typeStr = "TOKEN_STRING"; break;
        case TOKEN_EOF: typeStr = "TOKEN_EOF"; break;
        case TOKEN_UNKNOWN: typeStr = "TOKEN_UNKNOWN"; break;
    }

    if (
        t.type == TOKEN_KEYWORD ||
        t.type == TOKEN_IDENTIFIER ||
        t.type == TOKEN_STRING
        )
        printf("%s: "SV_Fmt"\n", typeStr, SV_Arg(t.content));
    else
        printf("%s\n", typeStr);
}
