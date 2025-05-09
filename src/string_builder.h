#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// Allocated strings
typedef struct {
    char *data;
    size_t len;
    size_t alloc;
} StringBuilder;

StringBuilder *sb_new();
StringBuilder *sb_from_size(size_t size);
StringBuilder *sb_from_parts(const char* data, size_t len);
StringBuilder *sb_from_cstr(const char *cstr);
void sb_free(StringBuilder *s);

void sb_append_cstr(StringBuilder *s, const char *cstr);
void sb_appendf(StringBuilder *s, const char *fmt, ...);
void sb_append_char(StringBuilder *s, char c);
const char *sb_get_cstr(StringBuilder *s);

#ifdef SB_IMPL
// String implemntation

StringBuilder *sb_new() {
    StringBuilder *s = malloc(sizeof(StringBuilder));
    s->data = NULL;
    s->len = 0;
    s->alloc = 16; // default allocation size
    s->data = malloc(s->alloc);

    return s;
}

StringBuilder *sb_from_size(size_t size) {
    StringBuilder *s = malloc(sizeof(StringBuilder));
    s->data = NULL;
    s->len = 0;
    s->alloc = size;
    s->data = malloc(s->alloc);

    return s;
}

StringBuilder *sb_from_parts(const char* data, size_t len) {
    StringBuilder *s = sb_from_size(len);

    memcpy(s->data, data, len);
    s->len = len;

    return s;
}

StringBuilder *sb_from_cstr(const char *cstr) {
    return sb_from_parts(cstr, strlen(cstr));
}

void sb_free(StringBuilder *s) {
    free(s->data);
    s->data = NULL;
    s->len = 0;
    s->alloc = 0;

    free(s);
}

void sb_realloc_if_required(StringBuilder *s, size_t required_size) {
    while (s->alloc < required_size) {
        if (s->alloc == 0) exit(1);

        s->alloc = s->alloc * 2;
        s->data = realloc(s->data, s->alloc);
    }
}

void sb_append_cstr(StringBuilder *s, const char *cstr) {
    size_t cstr_len = strlen(cstr);
    size_t required_len = s->len + cstr_len;
    sb_realloc_if_required(s, required_len);
    
    memcpy(s->data + s->len, cstr, cstr_len);
    s->len = required_len;
}

void sb_appendf(StringBuilder *s, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // size needed for format string
    int format_len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (format_len <= 0) return;

    sb_realloc_if_required(s, s->len + format_len + 1);

    // write formatted string directly into string builder buffer
    va_start(args, fmt);
    vsnprintf(s->data + s->len, format_len + 1, fmt, args);
    va_end(args);

    s->len += format_len;
}

void sb_append_char(StringBuilder *s, char c) {
    sb_realloc_if_required(s, s->len + 1);

    s->data[s->len] = c;
    s->len++;
}

const char *sb_get_cstr(StringBuilder *s) {
    // make sure we have space for null-term
    sb_realloc_if_required(s, s->len + 1);

    if (s->data[s->len] != '\0') {
        // append null terminator
        s->data[s->len] = '\0';
    }

    return s->data;
}
#endif // SB_IMPL
