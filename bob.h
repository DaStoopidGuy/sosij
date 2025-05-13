#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>

// macros for stripping prefixes

#ifdef BOB_STRIP_PREFIXES
#define BOB_STRING_BUILDER_STRIP
#define BOB_CMD_STRIP
#define rebuild_yourself bob_rebuild_yourself
#endif // BOB_STRIP_PREFIXES

#ifdef BOB_STRING_BUILDER_STRIP
#define StringBuilder Bob_StringBuilder
#define sb_create bob_sb_create
#define sb_from_parts bob_sb_from_parts
#define sb_from_cstr bob_sb_from_cstr
#define sb_from_sb bob_sb_from_sb
#define sb_destroy bob_sb_destroy
#define sb_get_cstr bob_sb_get_cstr
#define sb_append_parts bob_sb_append_parts
#define sb_append_sb bob_sb_append_sb
#define sb_append_cstr bob_sb_append_cstr
#endif // BOB_STRING_BUILDER_STRIP

#ifdef BOB_CMD_STRIP
#define Cmd Bob_Cmd
#define cmd_append bob_cmd_append
#define cmd_destroy bob_cmd_destroy
#define cmd_run bob_cmd_run
#define cmd_run_and_reset bob_cmd_run_and_reset
#define cmd_run_silent bob_cmd_run_silent
#define cmd_run_and_reset_silent bob_cmd_run_and_reset_silent
#define check_dir_exists bob_check_dir_exists
#define check_file_exists bob_check_file_exists
#endif // BOB_CMD_STRIP

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} Bob_StringBuilder;

Bob_StringBuilder bob_sb_create();
Bob_StringBuilder bob_sb_from_parts(const char *data, size_t len);
Bob_StringBuilder bob_sb_from_cstr(const char *str);
Bob_StringBuilder bob_sb_from_sb(Bob_StringBuilder *sb);
void bob_sb_destroy(Bob_StringBuilder *sb);
const char *bob_sb_get_cstr(Bob_StringBuilder *sb);
void bob_sb_append_parts(Bob_StringBuilder *sb, const char *data, size_t len);
void bob_sb_append_sb(Bob_StringBuilder *primary, Bob_StringBuilder *secondary);
void bob_sb_append_cstr(Bob_StringBuilder *sb, const char* str);

typedef struct {
    Bob_StringBuilder *items;
    size_t len;
    size_t cap;
} Bob_Cmd;

#define bob_cmd_append(cmd, ...) _bob_cmd_append_many( \
        (cmd),                                        \
        (const char *[]){ __VA_ARGS__ },              \
        sizeof ( (const char *[]){ __VA_ARGS__ } ) / sizeof (const char *))

void _bob_cmd_append_many(Bob_Cmd *cmd, const char *args[], int args_count);
void bob_cmd_destroy(Bob_Cmd *cmd);

bool bob_cmd_run_silent(Bob_Cmd *cmd);
bool bob_cmd_run_and_reset_silent(Bob_Cmd *cmd);
bool bob_cmd_run(Bob_Cmd *cmd);
bool bob_cmd_run_and_reset(Bob_Cmd *cmd);

void bob_rebuild_yourself(int argc, char *argv[]);

bool bob_check_dir_exists(const char *dirname);
bool bob_check_file_exists(const char *filename);

//----------------------//
// Dynamic Array Macros //
//----------------------//

/*
 * The `da` argument should be a struct like following:
 * typedef struct {
 *  <TypeName> *items;
 *  size_t len;
 *  size_t cap;
 * } Dynamic_Array_Struct;
 *
 * NOTE - initialize as follows:
 *  Dynamic_Array_Struct da = {0};  // to make sure all fields are zero-initialized
 */

#define BOB_DA_INITIAL_CAP 8

#define bob_da_free(da)    \
    do {                   \
        free((da)->items); \
        (da)->len = 0;     \
        (da)->cap = 0;     \
    } while(0)             \

#define bob_da_append(da, item)                                                     \
    do {                                                                            \
        if ((da)->len >= (da)->cap)                                                 \
        {                                                                           \
            (da)->cap = (da)->cap==0 ? BOB_DA_INITIAL_CAP : (da)->cap*2;            \
            (da)->items = realloc((da)->items, (da)->cap * sizeof(*(da)->items));   \
            assert((da)->items != NULL);                                            \
        }                                                                           \
        (da)->items[(da)->len++] = item;                                            \
    } while(0)

#define bob_da_reserve(da, needed_size)                                             \
    do {                                                                            \
        if((da)->cap < needed_size)                                                 \
        {                                                                           \
            (da)->cap = needed_size;                                                \
            (da)->items = realloc((da)->items, (da)->cap * sizeof(*(da)->items));   \
            assert((da)->items != NULL);                                            \
        }                                                                           \
    } while(0)

#ifdef BOB_IMPL
//----------------//
// String Builder //
//----------------//
Bob_StringBuilder bob_sb_create() {
    Bob_StringBuilder sb = {0};
    sb.cap = 16;
    sb.data = malloc(sb.cap);
    sb.len = 0;
    return sb;
}

Bob_StringBuilder bob_sb_from_parts(const char *data, size_t len) {
    Bob_StringBuilder sb = {0};
    sb.cap = len;
    sb.data = malloc(sb.cap + 1); // no null check for allocated memory
    memcpy(sb.data, data, len);
    sb.len = len;
    sb.data[sb.len] = '\0'; // null terminate
    return sb;
}

Bob_StringBuilder bob_sb_from_cstr(const char *str) {
    return bob_sb_from_parts(str, strlen(str));
}

Bob_StringBuilder bob_sb_from_sb(Bob_StringBuilder *sb) {
    return bob_sb_from_parts(sb->data, sb->len);
}

void bob_sb_destroy(Bob_StringBuilder *sb) {
    free(sb->data);
    sb->cap = 0;
    sb->len = 0;
}

void bob_sb_realloc_if_required(Bob_StringBuilder *sb, size_t required_size) {
    while (sb->cap < required_size) {
        sb->cap *= 2;
        sb->data = realloc(sb->data, sb->cap);
    }
}

const char *bob_sb_get_cstr(Bob_StringBuilder *sb) {
    bob_sb_realloc_if_required(sb, sb->len+1);

    if (sb->data[sb->len] != '\0')
        sb->data[sb->len] = '\0';

    return sb->data;
}

void bob_sb_append_parts(Bob_StringBuilder *sb, const char *data, size_t len) {
    size_t required_len = sb->len + len;
    bob_sb_realloc_if_required(sb, required_len + 1);
    memcpy(sb->data + sb->len, data, len);
    sb->len = required_len;
    // maintain null terminator
    sb->data[sb->len] = '\0';
}

void bob_sb_append_sb(Bob_StringBuilder *primary, Bob_StringBuilder *secondary) {
    bob_sb_append_parts(primary, secondary->data, secondary->len);
}

void bob_sb_append_cstr(Bob_StringBuilder *sb, const char* str) {
    bob_sb_append_parts(sb, str, strlen(str));
}

//------------------------//
// Bob Command or Bob_Cmd //
//------------------------//

void _bob_cmd_append_many(Bob_Cmd *cmd, const char *args[], int args_count) {
    for (int i=0; i<args_count; i++) {
        const char *arg = args[i];

        // make string builder from argument
        // and append to cmd 'args'
        Bob_StringBuilder sb_arg = bob_sb_from_cstr(arg);
        bob_da_append(cmd, sb_arg);
    }
}

void bob_cmd_destroy(Bob_Cmd *cmd) {
    for (size_t i=0; i<cmd->len; i++) {
        bob_sb_destroy(&cmd->items[i]);
    }
    bob_da_free(cmd);
}

Bob_StringBuilder bob_get_cmd_string(Bob_Cmd *cmd) {
    Bob_StringBuilder complete_cmd = bob_sb_from_sb(&cmd->items[0]);

    for (size_t i=1; i<cmd->len; i++) {
        Bob_StringBuilder *arg = &cmd->items[i];
        bob_sb_append_cstr(&complete_cmd, " ");
        bob_sb_append_sb(&complete_cmd, arg);
    }

    return complete_cmd;
}

// returns true on success, false on failure
bool bob_cmd_run_silent(Bob_Cmd *cmd) {
    if (cmd->len == 0) return false;
    Bob_StringBuilder cmd_string = bob_get_cmd_string(cmd);

    int ret = system(bob_sb_get_cstr(&cmd_string));
    if (ret != 0) {
        printf("Following command could not be run successfully:\n>   %s\n", bob_sb_get_cstr(&cmd_string));
        return false;
    }

    bob_sb_destroy(&cmd_string);
    return true;
}

bool bob_cmd_run_and_reset_silent(Bob_Cmd *cmd) {
    bool status = bob_cmd_run_silent(cmd);
    cmd->len = 0;
    return status;
}

bool bob_cmd_run(Bob_Cmd *cmd) {
    Bob_StringBuilder cmd_string = bob_get_cmd_string(cmd);
    printf("> %s\n", bob_sb_get_cstr(&cmd_string));
    bob_sb_destroy(&cmd_string);
    return bob_cmd_run_silent(cmd);
}

bool bob_cmd_run_and_reset(Bob_Cmd *cmd) {
    bool status = bob_cmd_run(cmd);
    cmd->len = 0;
    return status;
}

void bob_rebuild_yourself(int argc, char *argv[]) {
    struct stat exe_stat, src_stat;
    stat(argv[0], &exe_stat);
    stat("bob.c", &src_stat);

    if (src_stat.st_mtim.tv_sec  > exe_stat.st_mtim.tv_sec ||
       (src_stat.st_mtim.tv_sec == exe_stat.st_mtim.tv_sec &&
        src_stat.st_mtim.tv_nsec > exe_stat.st_mtim.tv_nsec)
    ) {
        // src is newer that executable
        printf("REBUILDING BOB...\n");
        Bob_Cmd cmd = {0};
        bob_cmd_append(&cmd, "gcc", "-o", "bob", "bob.c");
        bool compiled_successfully = bob_cmd_run_and_reset_silent(&cmd);

        bob_cmd_append(&cmd, "./bob");
        if (argc > 1) {
            for (int i=1; i<argc; i++) {
                bob_cmd_append(&cmd, argv[i]);
            }
        }

        // run new bob version
        if (compiled_successfully)
            bob_cmd_run_and_reset_silent(&cmd);

        bob_cmd_destroy(&cmd);
        exit(0);
    }
}

bool bob_check_file_exists(const char *filename) {
    struct stat file_stat;
    int status = stat(filename, &file_stat);
    if (status != 0) // does not exist
        return false;

    // returns if it is a regular file
    return S_ISREG(file_stat.st_mode);
}

bool bob_check_dir_exists(const char *dirname) {
    struct stat dir_stat;
    int status = stat(dirname, &dir_stat);
    if (status != 0) // does not exist lol
        return false;

    // return if its a directory
    return S_ISDIR(dir_stat.st_mode);
}
#endif // BOB_IMPL
