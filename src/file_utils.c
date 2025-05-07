#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file_to_buffer(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file); // or fseek(file, 0, SEEK_SET);

    if (size < 0) {
        perror("ftell");
        fclose(file);
        return NULL;
    }

    // Allocate buffer (+1 for null terminator if text)
    char* buffer = malloc(size + 1);
    if (!buffer) {
        perror("malloc");
        fclose(file);
        return NULL;
    }

    // Read entire file
    size_t read_size = fread(buffer, 1, size, file);
    if (read_size != (size_t)size) {
        perror("fread");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[size] = '\0'; // Null-terminate in case it's text
    fclose(file);

    return buffer;
}
