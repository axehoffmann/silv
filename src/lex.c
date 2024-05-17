#include "lex.h"

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "hash_generator.h"
#include "keyword_hashes.h"
#include "compiler_util.h"

Token make_tkn(Lex* l, u8 width, Token tk) {
    l->index += width;
    l->curCol += width;
    return tk;
}

#define tkn(N, TY) make_tkn(l, N, (Token){      \
    .type = TY,                                 \
    .line = l->curLn,                           \
    .column = l->curCol,                        \
    .index = l->index,                          \
});

bool isident(char c) {
    return c == '_' || isalpha(c) || isdigit(c);
}

Token lex_id_kw(Lex* l) {
    // Find length of the kw/ident
    u32 width = 1;
    while (isident(*(l->buffer.data + l->index + width))) width++;
    
    char* loc = (l->buffer.data + l->index);
    // Compare hash of the string for keyword lookup.
    switch (hash_str(loc, width, 0)) {
#define kw(HASH, TK, TKSTR)                             \
    case HASH: {                                        \
        const u64 tklen = strlen(TKSTR);                \
        if (tklen != width) break;                      \
        if (strncmp(loc, TKSTR, strlen(TKSTR)) != 0) break; \
        return tkn(width, TK); }

    kw(KW_FN_HASH,     FN,     "fn");
    kw(KW_STRUCT_HASH, STRUCT, "struct");

    kw(KW_U8_HASH,     U8,     "u8");
    kw(KW_U16_HASH,    U16,    "u16");
    kw(KW_U32_HASH,    U32,    "u32");
    kw(KW_U64_HASH,    U64,    "u64");
    kw(KW_I8_HASH,     I8,     "i8");
    kw(KW_I16_HASH,    I16,    "i16");
    kw(KW_I32_HASH,    I32,    "i32");
    kw(KW_I64_HASH,    I64,    "i64");
    kw(KW_F32_HASH,    F32,    "f32");
    kw(KW_F64_HASH,    F64,    "f64");
    kw(KW_VEC2_HASH,   VEC2,   "vec2");
    kw(KW_VEC3_HASH,   VEC3,   "vec3");
    kw(KW_VEC4_HASH,   VEC4,   "vec4");
    kw(KW_MAT2_HASH,   MAT2,   "mat2");
    kw(KW_MAT3_HASH,   MAT3,   "mat3");
    kw(KW_MAT4_HASH,   MAT4,   "mat4");

    kw(KW_BOOL_HASH,   BOOL,   "bool");
    kw(KW_TRUE_HASH,   BTRUE,  "true");
    kw(KW_FALSE_HASH,  BFALSE, "false");

    kw(KW_IF_HASH,     IF,     "if");
    kw(KW_ELSE_HASH,   ELSE,   "else");

    kw(KW_FOR_HASH,    FOR,    "for");
    kw(KW_IN_HASH,     IN,     "in");
    kw(KW_RETURN_HASH, RETURN, "return");
    kw(KW_BREAK_HASH,  BREAK,  "break");
    kw(KW_CONTINUE_HASH, CONTINUE, "continue");

    kw(KW_VOID_HASH,   VOID,   "void");

    default: break;
    }
#undef kw

    u32 i = l->index;
    l->index += width;
    l->curCol += width;
    return (Token){
        .type = IDENT,
        .line = l->curLn, .column = l->curCol, .index = l->index,
        .str = (Str){
            .data = loc,
            .size = width,
        },
    };
}

Token lex_num(Lex* l) {

    u8 width = 1;
    while (isdigit(*(l->buffer.data + l->index + width))) width++;

    if (*(l->buffer.data + l->index + width) == '.') {
        // Its a decimal! parse it as a float
        width++;
        while (isdigit(*(l->buffer.data + l->index + width))) width++;
        char* end;
        f64 val = strtod(l->buffer.data + l->index, &end);
        assert(end == (l->buffer.data + l->index + width));
        assert(errno == 0);

        Token tk = (Token){
            .type = FLOAT,
            .line = l->curLn, .column = l->curCol, .index = l->index,
            .fpoint = val,
        };
        l->index += width;
        l->curCol += width;
        return tk;
    }

    // Parse as an integer.
    char* end;
    u64 val = strtoull(l->buffer.data + l->index, &end, 0);
    assert(end == (l->buffer.data + l->index + width));
    assert(errno == 0);

    Token tk = (Token){
        .type = INTEGER,
        .line = l->curLn, .column = l->curCol, .index = l->index,
        .uint = val,
    };
    l->index += width;
    l->curCol += width;
    return tk;
}

Token lex_tkn(Lex* l) {
    // #TODO why
    if (l->index >= l->buffer.size)
        return (Token){ .type = EOF_TOK };

    char c = *(l->buffer.data + l->index);

    switch (c) {
    // Whitespace
    case '\n':
        l->curLn++;
        l->curCol = 0;
    case ' ':
    case '\t':
        l->index++;
        l->curCol++;
        return lex_tkn(l);

    case '.': return tkn(1, DOT);
    case ':': 
        switch (*(l->buffer.data + l->index + 1)) {
        case ':': return tkn(2, CONST_ASGN);
        default: return tkn(1, COLON);
        }
    case ',': return tkn(1, COMMA);
    case ';': return tkn(1, SEMI);
    case '{': return tkn(1, LBRACE);
    case '}': return tkn(1, RBRACE);
    case '[': return tkn(1, LBRACK);
    case ']': return tkn(1, RBRACK);
    case '(': return tkn(1, LPAREN);
    case ')': return tkn(1, RPAREN);

#define lex_op(C, TK)                                   \
    case C:                                             \
        if (*(l->buffer.data + l->index + 1) == '=')    \
            return tkn(2, TK##_ASGN);                   \
        return tkn(1, TK);                              

#define lex_boolop(C, TK)                               \
    case C:                                             \
        switch (*(l->buffer.data + l->index + 1)) {     \
        case '=': return tkn(2, TK##_ASGN);             \
        case C: return tkn(1, TK);                      \
        default: return tkn(1, TK##_BIT);               \
        }                                               \

    // #TODO: strtoll for signed integers? or just handle signed ints in parsing
    lex_op('+', ADD)
    lex_op('*', MUL)
    lex_op('/', DIV)
    lex_op('%', MOD)
    lex_boolop('|', OR)
    lex_boolop('&', AND)
    case '~': return tkn(1, NOT_BIT)

    // Can't macro for sub, because -> token exists
    // -=, ->, -
    case '-':
        switch (*(l->buffer.data + l->index + 1)) {
        case '=': return tkn(2, SUB_ASGN);
        case '>': return tkn(2, ARROW);
        default:  return tkn(1, SUB);
        }

    // Comparison operators
    case '!':
        switch (*(l->buffer.data + l->index + 1)) {
        case '=': return tkn(2, NEQ);
        default: return tkn(1, NOT);
        }
    case '=':
        switch (*(l->buffer.data + l->index + 1)) {
        case '=': return tkn(2, EQ);
        default: return tkn(1, ASGN);
        }
    case '>':
        switch (*(l->buffer.data + l->index + 1)) {
        case '=': return tkn(2, GEQ);
        default: return tkn(1, GT);
        }
    case '<':
        switch (*(l->buffer.data + l->index + 1)) {
        case '=': return tkn(2, LEQ);
        default: return tkn(1, LT);
        }

    case EOF:
    case '\0':
        return tkn(1, EOF_TOK);

    default:
        // Try lex an identifier or keyword
        if (isalpha(c)) return lex_id_kw(l);
        // Is it a number?
        if (isdigit(c)) return lex_num(l);
    }

    fprintf(stderr, "\nError on line %u (column %u): unknown token  '%c'\n\n", l->curLn, l->curCol, c);
    errloc(l->buffer.data, l->index, l->curLn);
    exit(1);
}

#undef lex_op
#undef lex_boolop
#undef tkn

void refill_lex_buf(Lex* l) {
    while (l->tkWrite != l->tkRead) {
        l->tkbuf[l->tkWrite] = lex_tkn(l);
        l->tkWrite = (l->tkWrite + 1) % LEX_BUFFER_SIZE;
    }
}

Lex* lex_start(char* file) {
    Str data;
    if (!load_file(file, &data)) {
        fprintf(stderr, "Failed to load file: %s", file);
        return NULL;
    }

    Lex* l = new(Lex);
    *l = (Lex){
        .buffer = data,
        .index = 0,

        .tkWrite = 0,
        .tkRead = 0,

        .curLn = 1, // Editors start at line 1
        .curCol = 0,
    };
    // Fill initial buffer
    for (u32 i = 0; i < LEX_BUFFER_SIZE; i++) {
        l->tkbuf[i] = lex_tkn(l);
    }

    return l;
}

void lex_end(Lex* l) {
    str_free(&l->buffer);
    free(l);
}

Token lex_eat(Lex* l) {
    refill_lex_buf(l);

    Token tk = l->tkbuf[l->tkRead];
    l->tkRead = (l->tkRead + 1) % LEX_BUFFER_SIZE;

    return tk;
}

Token lex_peek(Lex* l, u8 offset) {
    assert(offset < LEX_BUFFER_SIZE);

    refill_lex_buf(l);
    return l->tkbuf[(l->tkRead + offset) % LEX_BUFFER_SIZE];
}

void lex_skip(Lex* l, u8 offset) {
    l->tkRead = (l->tkRead + offset) % LEX_BUFFER_SIZE;
}
