#pragma once
#include "arena.h"
#include "string_view.h"

//------------------//
// Type Definitions //
//------------------//

// here, node is what i am calling a distinct feature
// of the syntax
enum NodeType {
    NODE_STRING,
    // NODE_VAR_DECL,
    NODE_FUNC_CALL,
    NODE_FUNC_DEF,
};

typedef struct Node{
    enum NodeType type;
    union {
        // NODE_STRING
        struct {
            StringView content;
        } string;

        // NODE_FUNC_CALL
        struct {
            StringView name;
            struct {
                struct Node **items;
                size_t len;
                size_t cap;
            } args;
        } func_call;

        // NODE_FUNC_DEF
        struct {
            StringView name;
            struct {
                struct Node **items;
                size_t len;
            } args;
            enum NodeType ret_type;

            // function body statements
            struct {
                struct Node **items;
                size_t len;
            } stmts;
        } func_def;
    };
} Node;

void parser_init(Arena *, const char*);
Node* parse_statement();
Node *parse_func_def();
void parse_program();
void print_node(Node* node, int indent);
