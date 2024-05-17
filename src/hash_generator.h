#pragma once

#include "lib.h"
#include <stdio.h>
#include <string.h>

inline u32 hash_str(const char* str, u32 n, u64 idx) {
    return !(idx != n) ? 55 : (hash_str(str, n, idx + 1) * 33) + (u8)(str[idx]);
}

inline void generate_keyword_hashes() {

#define genhash(NAME, KW) printf("#define %s %u\n", #NAME, hash_str(KW, strlen(KW), 0))

    genhash(KW_FN_HASH, "fn");
    genhash(KW_STRUCT_HASH, "struct");

    genhash(KW_U8_HASH,    "u8");
    genhash(KW_U16_HASH,   "u16");
    genhash(KW_U32_HASH,   "u32");
    genhash(KW_U64_HASH,   "u64");
    genhash(KW_I8_HASH,    "i8");
    genhash(KW_I16_HASH,   "i16");
    genhash(KW_I32_HASH,   "i32");
    genhash(KW_I64_HASH,   "i64");
    genhash(KW_F32_HASH,   "f32");
    genhash(KW_F64_HASH,   "f64");
    
    genhash(KW_VEC2_HASH,  "vec2");
    genhash(KW_VEC3_HASH,  "vec3");
    genhash(KW_VEC4_HASH,  "vec4");
    genhash(KW_MAT2_HASH,  "mat2");
    genhash(KW_MAT3_HASH,  "mat3");
    genhash(KW_MAT4_HASH,  "mat4");

    genhash(KW_BOOL_HASH,  "bool");
    genhash(KW_TRUE_HASH,  "true");
    genhash(KW_FALSE_HASH, "false");

    genhash(KW_IF_HASH, "if");
    genhash(KW_ELSE_HASH, "else");
    genhash(KW_VOID_HASH, "void");

    genhash(KW_FOR_HASH, "for");
    genhash(KW_IN_HASH, "in");

    genhash(KW_RETURN_HASH, "return");
    genhash(KW_CONTINUE_HASH, "continue");
    genhash(KW_BREAK_HASH, "break");


    genhash(KW_SWITCH_HASH, "switch");
    genhash(KW_CASE_HASH, "case");

#undef genhash
}
