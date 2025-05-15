#include "codegen.h"
#include "file_utils.h"
#include "parser.h"
#include <stdlib.h>

int main() {
    const char* source = read_file_to_buffer("test.sosij");

    Arena parser_arena = {0};
    arena_init(&parser_arena, 1024*1024*10); // 10 mB arena
    parser_init(&parser_arena, source);

    Node *func = parse_func_def();
    //print_node(func, 0);

    FILE *outfile = fopen("out.s", "w");
    codegen_init(outfile);
    codegen_generate(func);
    codegen_write_and_deinit();
    fclose(outfile);

    // assemble and link
    system("nasm out.s -f elf64 -o out.o\n");
    system("ld out.o -o out\n");

    // clean up
    free((void *)source);
    arena_destroy(&parser_arena);

    return 0;
}
