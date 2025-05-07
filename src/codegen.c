#include "codegen.h"
#include "parser.h"
#include <stdio.h>

void generate(Node* node, FILE* out) {
    switch (node->type) {
        case NODE_FUNC_CALL:
            {
                if (sv_equals_cstr(node->func_call.name, "print")) {
                    // write .data section for hardcoded string
                    fprintf(out,
                            "section .data\n"
                            "msg db \"" SV_Fmt "\", 10\n"
                            "len equ $ - msg\n"
                            ,SV_Arg(node->func_call.args[0]->string.content));

                    // write .text section for instructions 
                    fprintf(out,
                            "section .text\n"
                            "global _start\n"
                            );
                    fprintf(out, 
                            "_start:\n"
                            "mov rax, 1\n" // rax = 1 is write syscall
                            "mov rdi, 1\n" // rdi needs file descriptor, rdi 1 for write is `stdout`
                            "mov rsi, msg\n" // buffer
                            "mov rdx, len\n" // length of buffer
                            "syscall\n"
                            );

                    fprintf(out,
                            "; exit(0)\n"
                            "mov rax, 60\n" // syscall exit - 0
                            "mov rdi, 0\n" // rdi - exit code
                            "syscall\n"
                            );
                }
            } break;
        default:
            return;
    }
}
