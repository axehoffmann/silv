#include "alloc.h"

#include <assert.h>
#include <stdlib.h>

struct arena {
    usize it;
    usize capacity;
    char buffer[];
};

Arena* arena_new(usize size) {
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

usize arena_mark(Arena* arena) {
    return arena->it;
}

void arena_restore(Arena* arena, usize mark) {
    arena->it = mark;
}

// based on https://codereview.stackexchange.com/a/291426
void* arena_allocate(Arena* arena, usize size, usize alignment) {
    assert(alignment == 1 || !(alignment & (alignment - 1)));
    assert(size / alignment * alignment == size);

    char* p = arena->buffer + arena->it;

    uintptr_t original = (uintptr_t)p;

    if (original > UINTPTR_MAX - alignment) {
        return NULL;
    }

    // Rounds the pointer up to a multiple of alignment
    uintptr_t aligned = (original + alignment - 1) & !(alignment - 1);

    size_t offset = aligned - original;

    if (size > SIZE_MAX - offset) {
        return NULL;
    }

    size += offset;

    if (size > arena->capacity - arena->it) {
        return NULL;
    }

    arena->it += size;

    return p + offset;
}
