#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;

typedef double   f64;
typedef float    f32;

typedef size_t   usize;

#define Array_for(ITEM, ARR) \
    for (__typeof__(ARR) ITEM = ARR; ITEM != ARR + arrlen(ARR); ITEM++)

struct Str {
    char* data;
    u32 size;
};
typedef struct Str Str;

void str_free(Str* str);

#define ref(X) _Generic(        \
        (X),                    \
        Str: str_ref            \
    )(X)

#define newarr(X, N)  assert_malloc(sizeof(X) * N)
#define new(X)        assert_malloc(sizeof(X))

#define newzarr(X, N) assert_calloc(N, sizeof(X))

// Allocates a str
#define newstr(N)  (Str){ .data = newarr(char, N), .size = N }
// Allocates a str, and zeroes the memory.
#define newzstr(N) (Str){ .data = newzarr(char, N), .size = N }

// Mallocs, or exits if OOM.
inline void* assert_malloc(usize size)
{
    void* data = malloc(size);
    if (!data)
    {
        fprintf(stderr, "Out of memory error\n");
        exit(1);
    }
    return data;
}

inline void* assert_calloc(usize n, usize size)
{
    void* data = calloc(n, size);
    if (!data)
    {
        fprintf(stderr, "Out of memory error\n");
        exit(1);
    }
    return data;
}

// Loads an entire file.
bool load_file(char* path, Str* out);
