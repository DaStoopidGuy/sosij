#pragma once
#include <stdlib.h>

typedef struct {
    void   *base;
    size_t size;
    size_t offset;
} Arena;

// creates an Arena with the provided size
void arena_init(Arena *a, size_t size);

// Destroy existing arena
// - frees Arena memory
void arena_destroy(Arena *a);

// Allocates memory on existing arena
// - returns pointer to allocated memory
void *arena_alloc(Arena *a, size_t size);

// Effectively resets the entire arena
// - does not actually deallocate any memory
// - subsequent arena_alloc() calls will overwrite
//   previously allocated memory
void arena_reset(Arena *a);

#ifdef ARENA_IMPL
void arena_init(Arena *a, size_t size)
{
    a->base = malloc(size);
    a->size = a->base ? size : 0;
    a->offset = 0;
}

void arena_destroy(Arena *a)
{
    free(a->base);
    a->base = NULL;
    a->size = 0;
    a->offset = 0;
}

void *arena_alloc(Arena *a, size_t size)
{
    if (a->offset + size > a->size)
        return NULL; // out of memory

    void *ptr = (char*)a->base +a->offset;
    a->offset += size;
    return ptr;
}

void arena_reset(Arena *a)
{
    a->offset = 0;
}

#endif  // ARENA_IMPL
