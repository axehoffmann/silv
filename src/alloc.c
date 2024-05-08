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

void* arena_alloc(Arena* arena, u64 size, u64 alignment) {
    assert(alignment == 1 || !(alignment & (alignment - 1)));
    assert(size / alignment * alignment == size);

    char* p = arena->pool + arena->count;

    uintptr_t original = ((uintptr_t)p);

    if (original > UINTPTR_MAX - alignment) {
        return nullptr;
    }

    /*
       Rounds up the pointer value to the next multiple of alignment.

       - With alignment being a power of 2, alignment - 1 is of the form
         0b001..1.
       - Thus "x & !(alignment - 1)" rounds x down to the previous multiple of
         alignment.
       - And "(x + alignment - 1) & !(alignment -1)" rounds up.
    */
    uintptr_t aligned = (original + alignment - 1) & !(alignment - 1);

    size_t offset = aligned - original;

    if (size > SIZE_MAX - offset) {
        return nullptr;
    }

    size += offset;

    if (size > arena->capacity - arena->count) {
        return nullptr;
    }

    arena->count += size;

    /* Equal to "aligned", but preserves provenance */
    return p + offset;
}
}
