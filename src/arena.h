#pragma once
#include <stdlib.h>

typedef struct {
    void   *base;
    size_t size;
    size_t offset;
} Arena;

// creates an Arena with the provided size
// - allocates memory (needs to be destroyed)
// - returns pointer to allocated Arena struct
Arena *arena_create(size_t size);

// Destroy existing arena
// - frees Arena memory
// - frees Arena struct
void arena_destroy(Arena *a);

// Allocates memory on existing arena
// - returns pointer to allocated memory
void *arena_alloc(Arena *a, size_t size);

// Effectively resets the entire arena
// - does not actually deallocate any memory
// - subsequent arena_alloc() calls will overwrite
//   previously allocated memory
void arena_reset(Arena *a);

#ifdef ARENA_IMPLEMENTATION
Arena *arena_create(size_t size)
{
    Arena* a = malloc(sizeof(Arena));
    if (!a) return NULL;

    a->base = malloc(size);
    if (!a->base)
    {
        free(a);
        return NULL;
    }

    a->size = size;
    a->offset = 0;
    return a;
}

void arena_destroy(Arena *a)
{
    free(a->base);
    free(a);
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

#endif  // ARENA_IMPLEMENTATION
