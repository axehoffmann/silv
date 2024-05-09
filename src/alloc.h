#include "lib.h"

typedef struct arena Arena;

Arena* arena_new(usize size);
void arena_free(Arena* arena);

void arena_reset(Arena* arena);

void* arena_allocate(Arena* arena, usize size, usize alignment);

usize arena_mark(Arena* arena);
void arena_restore(Arena* arena, usize mark);

#define arena_alloc(A, T) arena_allocate(A, sizeof(T), _Alignof(T))
