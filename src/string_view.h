#pragma once
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// String view
#define SV_Fmt "%.*s"
#define SV_Arg(s) (int32_t) (s).len, (s).data

typedef struct {
    const char *data;
    size_t len;
} StringView;

StringView sv_from_parts(const char *data, size_t len);
StringView sv_from_cstr(const char *cstr);

StringView sv_trim_left(StringView s);
StringView sv_trim_right(StringView s);
StringView sv_trim(StringView s);

StringView sv_chop_by_delim(StringView *s, char delim);
bool sv_equals(StringView a, StringView b);
bool sv_equals_cstr(StringView a, const char *b);

#ifdef SV_IMPL
// String view implementation
StringView sv_from_parts(const char *data, size_t len) {
    StringView s = {
        .data = data,
        .len = len,
    };

    return s;
}

StringView sv_from_cstr(const char *cstr) {
    return sv_from_parts(cstr, strlen(cstr));
}

// trim all whitespace from left side and return trimmed stink
StringView sv_trim_left(StringView s) {
    size_t i = 0;

    // find first non-whitespace character
    while (i < s.len && isspace(s.data[i]))
    {
        i++;
    }

    return sv_from_parts(s.data + i, s.len - i);
}

// trim all whitespace from right side and return trimmed stink
StringView sv_trim_right(StringView s) {
    size_t i = 0;

    // find last non-whitespace character
    while (i < s.len && isspace(s.data[s.len - i - 1])) {
        i++;
    }

    return sv_from_parts(s.data, s.len - i);
}

// trim all surrounding whitespace
StringView sv_trim(StringView s) {
    return sv_trim_right(sv_trim_left(s));
}

StringView sv_chop_by_delim(StringView *s, char delim) {
    // find delimiter
    size_t i = 0;
    while (i < s->len && s->data[i] != delim) {
        i++;
    }

    StringView result = sv_from_parts(s->data, i);

    if (i < s->len) {
        s->data += i + 1;
        s->len  -= i + 1;
    } else {
        s->data += i;
        s->len -= i;
    }

    return result;
}

bool sv_equals(StringView a, StringView b) {
    if (a.len != b.len) 
        return false;
    else
        return memcmp(a.data, b.data, a.len) == 0;
}

bool sv_equals_cstr(StringView a, const char *b) {
    return sv_equals(a, sv_from_cstr(b));
}
#endif // SV_IMPL
