#include "parser.h"
#include "arena.h"
#include "lexer.h"
#include <stdio.h>
#include <assert.h>


//--------------//
// Global state //
//--------------//
Arena *arena;
Token current_token;

void next_token() {
    current_token = token_next();
}

//-----------//
// Functions //
//-----------//

// checks and advances token, error on not matching
Node *node_string(Token str) {
    Node *n = arena_alloc(arena, sizeof(Node));
    n->type = NODE_STRING;
    n->string.content = sv_from_parts(str.data, str.len);

    return n;
}

Node *node_func_call(Token name, Node *arg) {
    Node *n = arena_alloc(arena, sizeof(Node));
    n->type = NODE_FUNC_CALL;
    n->func_call.name.data = name.data;
    n->func_call.name.len = name.len;

    n->func_call.args = arena_alloc(arena, sizeof(Node *));
    n->func_call.args[0] = (struct Node*)arg;
    n->func_call.arg_count = 1;

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
    current_token = token_next();
}

Node* parse_func_call(Token name) {
    expect(TOKEN_LPAREN);

    // HACK: supports only string argument
    assert(current_token.type == TOKEN_STRING && "String type as func argument implemented only!");

    // Token str = current_token;
    // expect(TOKEN_STRING);
    //
    // Node *arg = malloc(sizeof(Node));
    // arg->type = NODE_STRING;
    // arg->string.data = str.data;
    // arg->string.len  = str.len;
    const Token string_token = current_token;
    expect(TOKEN_STRING);
    Node *arg = node_string(string_token);

    Node *fc = node_func_call(name, arg);

    expect(TOKEN_RPAREN);

    return fc;
}

Node* parse_statement() {
    Node *n;
    if (current_token.type == TOKEN_IDENTIFIER) {
        Token name = current_token;

        next_token();
        if (current_token.type == TOKEN_LPAREN) {
            n = parse_func_call(name);
            expect(TOKEN_SEMICOLON);
            return n;
        }

    }

    assert(false && "Weird ahh statement, can not recognize");
    return NULL;
}

// NOTE: print_indent and print_node are chatgpt ahh functions
void print_indent(int32_t level) {
    for (int32_t i = 0; i < level; i++) {
        printf("  ");
    }
}

void print_node(Node* node, int32_t indent) {
    if (!node) return;

    switch (node->type) {
        case NODE_STRING:
            print_indent(indent);
            printf("String: \""SV_Fmt"\n", SV_Arg(node->string.content));
            break;

        case NODE_FUNC_CALL:
            print_indent(indent);
            printf("Function Call: %.*s\n", (int32_t)node->func_call.name.len, node->func_call.name.data);
            for (int32_t i = 0; i < node->func_call.arg_count; i++) {
                print_node(node->func_call.args[i], indent + 1);
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
