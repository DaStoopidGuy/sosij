#pragma once
#include "arena.h"
#include <stdint.h>
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
            struct Node** args;
            int32_t arg_count;
        } func_call;
    };
} Node;

void parser_init(Arena *, const char*);
Node* parse_statement();
void parse_program();
void print_node(Node* node, int32_t indent);
