#include "alloc.h"

#include <stdlib.h>

struct arena {
    u64 it;
    u64 capacity;
    char buffer[];
};

Arena* arena_new(u64 size) {
    Arena* arena = malloc(sizeof(Arena) + size);
    arena->it = 0;
    arena->capacity = size;
    return arena;
}

void arena_free(Arena* arena) {
    free(arena);
}

void arena_reset(Arena* arena) {
    arena->it = 0;
}

void* arena_alloc(Arena* arena, u64 size, u64 alignment);
