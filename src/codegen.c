#include <assert.h>
#include <stdio.h>
#include "codegen.h"
#include "parser.h"
#include "string_builder.h"
#include "string_view.h"

FILE *out;

StringBuilder *data;
StringBuilder *start;

void write_data_section() {
    fprintf(out, 
            "section .data\n%s\n", sb_get_cstr(data)
            );
}

void write_text_section() {
    fprintf(out,
            "section .text\n"
            "global _start\n"
            );

    // entry point
    fprintf(out,
            "_start:\n%s\n", sb_get_cstr(start)
            );

    // exit(<error-code>)
    fprintf(out,
            "; exit(0)\n"
            "mov rax, 60\n" // syscall exit - 0
            "mov rdi, 0\n" // rdi - exit code
            "syscall\n"
           );
}

void emit_string_literal(StringBuilder *sb, StringView sv) {
    for (int i=0; i<sv.len; i++) {
        char c = sv.data[i];

        switch (c) {
            case '\n':
                sb_appendf(sb, "10");
                break;
            case '\"':
                sb_appendf(sb, "'\"'");
                break;
            case '\\':
                sb_appendf(sb, "'\\'");
                break;
            default:
                sb_appendf(sb, "'%c'", c);
        }

        if (i < sv.len - 1)
            sb_appendf(sb, ", ");
    }
}

void generate(Node* node, FILE* outfile) {
    out = outfile;
    data = sb_new();
    start = sb_new();

    switch (node->type) {
        case NODE_FUNC_CALL:
            {
                if (sv_equals_cstr(node->func_call.name, "print")) {
                    sb_appendf(data,
                            "msg db "
                            );
                    emit_string_literal(data, node->func_call.args[0]->string.content);
                    sb_appendf(data, 
                            "\n"
                            "len equ $ - msg\n"
                            );

                    // write _start label (entry point instruction)
                    sb_appendf(start, 
                            "mov rax, 1\n" // rax = 1 is write syscall
                            "mov rdi, 1\n" // rdi needs file descriptor, rdi 1 for write is `stdout`
                            "mov rsi, msg\n" // buffer
                            "mov rdx, len\n" // length of buffer
                            "syscall\n"
                            );
                }
            } break;
        default:
            assert(false && "Unimplemented NODE detected");
            break;
    }
    write_data_section();
    write_text_section();

    sb_free(data);
    sb_free(start);
}
