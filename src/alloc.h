#include "lib.h"

typedef struct arena Arena;

Arena* arena_new(u64 size);
void arena_free(Arena* arena);

void arena_reset(Arena* arena);

void* arena_alloc(Arena* arena, u64 size, u64 alignment);
