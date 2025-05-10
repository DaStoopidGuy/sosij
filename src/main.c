#include "codegen.h"
#include "file_utils.h"


int32_t main() {
    const char* source = read_file_to_buffer("test.sosij");

    Arena parser_arena;
    arena_create(&parser_arena, 1024*1024*10); // 10 mB arena
    parser_init(&parser_arena, source);

    Node *stmt = parse_statement();

    FILE *outfile = fopen("out.s", "w");
    generate(stmt, outfile);
    fclose(outfile);

    system("nasm out.s -f elf64 -o out.o\n");
    system("ld out.o -o out\n");

    // clean up
    arena_destroy(&parser_arena);
    free((void *)source);

    return 0;
}
