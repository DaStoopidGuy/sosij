#include "parser.h"
#include "arena.h"
#include "lexer.h"
#include "string_view.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../bob.h"

//--------------//
// Global state //
//--------------//
Arena *arena;
Token current_token;

void next_token() {
    current_token = token_next(arena);
}

//-----------//
// Functions //
//-----------//

// checks and advances token, error on not matching
Node *node_string(Token str) {
    Node *n = arena_alloc(arena, sizeof(Node));
    n->type = NODE_STRING;
    n->string.content = str.content;

    return n;
}

Node *node_func_call(Token name, Node *arg) {
    Node *n = arena_alloc(arena, sizeof(Node));
    n->type = NODE_FUNC_CALL;
    n->func_call.name = name.content;

    n->func_call.args.items = arena_alloc(arena, sizeof(Node *));
    n->func_call.args.items[0] = arg;
    n->func_call.args.len = 1;

    return n;
}

Node *node_func_def() {
    Node *n = arena_alloc(arena, sizeof(Node));
    n->type = NODE_FUNC_DEF;
    n->func_def.name = (StringView) {0};

    n->func_def.args.items = NULL;
    n->func_def.args.len = 0;

    n->func_def.stmts.items = NULL;
    n->func_def.stmts.len = 0;

    return n;
}

void expect(enum TokenType type) {
    // HACK: this is idiotic, need to properly handle error
    assert(current_token.type == type && "Error, token type not matching");
    next_token(); // advance
}

void parser_init(Arena *a, const char* input) {
    arena = a;

    lexer_init(input);
    current_token = token_next(arena);
}

Node* parse_func_call(Token name) {
    expect(TOKEN_LPAREN);

    // HACK: supports only string argument
    assert(current_token.type == TOKEN_STRING && "String type as func argument implemented only!");

    const Token string_token = current_token;
    expect(TOKEN_STRING);
    Node *arg = node_string(string_token);

    Node *fc = node_func_call(name, arg);

    expect(TOKEN_RPAREN);

    return fc;
}



Node* parse_statement() {
    Node *n;

    // skip leading whitespace (newline)
    while(current_token.type == TOKEN_NEWLINE)
        next_token();

    if (current_token.type == TOKEN_IDENTIFIER) {
        Token name = current_token;

        next_token();
        if (current_token.type == TOKEN_LPAREN) {
            n = parse_func_call(name);
            expect(TOKEN_NEWLINE);
            return n;
        }

    }

    return NULL;
}

Node *parse_func_def() {
    Node *n = node_func_def();
    if (current_token.type == TOKEN_IDENTIFIER) {
        n->func_def.name = current_token.content;

        next_token();
        // HACK: only supports zero function args
        expect(TOKEN_LPAREN);
        expect(TOKEN_RPAREN);
        expect(TOKEN_LBRACE);
        // func body
        struct {
            struct Node **items;
            size_t len;
            size_t cap;
        } stmts = {0};
        bob_da_init(&stmts);

        Node *stmt = parse_statement();
        while(stmt) {
            bob_da_append(&stmts, stmt);
            stmt = parse_statement();
        }

        n->func_def.stmts.items = arena_alloc(arena, sizeof(Node *) * stmts.len);
        n->func_def.stmts.len = stmts.len;
        for (size_t i=0; i<stmts.len; i++) {
            n->func_def.stmts.items[i] = stmts.items[i];
        }

        bob_da_free(&stmts);
        expect(TOKEN_RBRACE);
    }

    return n;
}

// NOTE: print_indent and print_node are chatgpt ahh functions
void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
}

void print_node(Node* node, int indent) {
    if (!node) return;

    switch (node->type) {
        case NODE_STRING:
            print_indent(indent);
            printf("String: \""SV_Fmt"\n", SV_Arg(node->string.content));
            break;

        case NODE_FUNC_CALL:
            print_indent(indent);
            printf("Function Call: %.*s\n", (int)node->func_call.name.len, node->func_call.name.data);
            for (size_t i = 0; i < node->func_call.args.len; i++) {
                print_node(node->func_call.args.items[i], indent + 1);
            }
            break;

        case NODE_FUNC_DEF:
            print_indent(indent);
            printf("Function Definition: "SV_Fmt"\nArgs:\n", SV_Arg(node->func_def.name));
            for (size_t i=0; i<node->func_def.args.len; i++) {
                print_node(node->func_def.args.items[i], indent + 1);
            }
            printf("Body Statements:\n");
            for (size_t i=0; i<node->func_def.stmts.len; i++) {
                print_node(node->func_def.stmts.items[i], indent + 1);
            }
            break;

        default:
            print_indent(indent);
            printf("Unknown node type\n");
            break;
    }
}

void parse_program() {
    while (current_token.type != TOKEN_EOF && current_token.type != TOKEN_UNKNOWN) {
        Node* stmt = parse_statement();
        print_node(stmt, 0);
    }
}
