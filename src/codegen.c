#include <assert.h>
#include <stdio.h>
#include "codegen.h"
#include "parser.h"
#include "string_builder.h"
#include "string_view.h"

FILE *out;

StringBuilder *data;
StringBuilder *text;

void write_data_section() {
    fprintf(out, 
            "section .data\n%s\n", sb_get_cstr(data)
            );
}

void write_text_section() {
    fprintf(out,
            "section .text\n"
            "global _start\n"
            "_start:\n"
            "jmp main\n"
            );

    // exit(<error-code>)
    fprintf(out,
            "exit_program:\n"
            "mov rax, 60\n" // syscall exit - 0
            "mov rdi, 0\n" // rdi - exit code
            "syscall\n"
           );

    // entry point
    fprintf(out, "%s\n", sb_get_cstr(text));
    fprintf(out, "jmp exit_program\n");
}

void emit_string_literal(StringBuilder *sb, StringView sv) {
    bool quote_started = false;

    for (size_t i=0; i<sv.len; i++) {
        char c = sv.data[i];

        switch (c) {
            case '\n':
                if (quote_started) {
                    sb_appendf(sb, "\"");
                    quote_started = false;
                    sb_appendf(sb, ", ");
                }
                sb_appendf(sb, "10");
                break;
            case '\"':
                if (quote_started) {
                    sb_appendf(sb, "\"");
                    quote_started = false;
                    sb_appendf(sb, ", ");
                }
                sb_appendf(sb, "'\"'");
                break;
            case '\\':
                if (quote_started) {
                    sb_appendf(sb, "\"");
                    quote_started = false;
                    sb_appendf(sb, ", ");
                }
                sb_appendf(sb, "'\\'");
                break;
            default:
                if (!quote_started) {
                    sb_appendf(sb, "\"");
                    quote_started = true;
                }
                sb_appendf(sb, "%c", c);
        }

        if (!quote_started && i < sv.len - 1)
            sb_appendf(sb, ", ");
    }

    // terminate string literal if missing end quote
    if (quote_started)
        sb_appendf(sb, "\"");
}

void codegen_generate(Node* node) {
    static int str_id = 0;

    switch (node->type) {
        case NODE_FUNC_CALL:
            {
                if (sv_equals_cstr(node->func_call.name, "print")) {
                    sb_appendf(data,
                            "str%d db ", str_id
                            );
                    emit_string_literal(data, node->func_call.args.items[0]->string.content);
                    sb_appendf(data, 
                            "\n"
                            "len%d equ $ - str%d\n", str_id, str_id
                            );

                    // write _start label (entry point instruction)
                    sb_appendf(text, 
                            "mov rax, 1\n" // rax = 1 is write syscall
                            "mov rdi, 1\n" // rdi needs file descriptor, rdi 1 for write is `stdout`
                            "mov rsi, str%d\n" // buffer
                            "mov rdx, len%d\n" // length of buffer
                            "syscall\n"
                            , str_id, str_id
                            );
                    str_id += 1;
                }
            } break;
        case NODE_FUNC_DEF:
            {
                sb_appendf(text,
                        SV_Fmt":\n",
                        SV_Arg(node->func_def.name)
                        );

                for (size_t i=0; i<node->func_def.stmts.len; i++) {
                    codegen_generate(node->func_def.stmts.items[i]);
                }
            } break;
        default:
            assert(false && "Unimplemented NODE detected");
            break;
    }

}

void codegen_init(FILE *outfile) {
    out = outfile;
    data = sb_new();
    text = sb_new();
}

void codegen_write_and_deinit() {
    write_data_section();
    write_text_section();
    sb_free(data);
    sb_free(text);
}
