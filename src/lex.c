#include "lex.h"

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

Token lex_id_kw(Lex* l)
{

}

Token lex_num(Lex* l)
{
    u8 width = 1;
    while (isdigit(*(l->buffer.data + l->index + width))) width++;
    // First figure out if its a float number or not.
    if (*(l->buffer.data + l->index + width) == '.')
    {
        width++;
        while (isdigit(*(l->buffer.data + l->index + width))) width++;
        char* end;
        f64 val = strtod(l->buffer.data + l->index, &end);
        assert(end == (l->buffer.data + l->index + width));
        assert(errno == 0);

        return (Token){
            .type = FLOAT,
            .line = l->curLn, .column = l->curCol, .index = l->index,
            .fpoint = val,
        };
    }

    char* end;
    u64 val = strtoull(l->buffer.data + l->index, &end, 0);
    assert(end == (l->buffer.data + l->index + width));
    assert(errno == 0);
    return (Token){
        .type = INTEGER,
        .line = l->curLn, .column = l->curCol, .index = l->index,
        .uint = val,
    };
}

Token make_tkn(Lex* l, u8 width, Token tk)
{
    l->index += width;
    return tk;
}

#define tkn(N, TY) make_tkn(l, N, (Token){      \
    .type = TY,                                 \
    .line = l->curLn,                           \
    .column = l->curCol,                        \
    .index = l->index,                          \
});

Token lex_tkn(Lex* l)
{
    char c = *(l->buffer.data + l->index);
    switch (c)
    {
    // Whitespace
    case '\n':
        l->curLn++;
        l->curCol = 0;
    case ' ':
    case '\t':
        l->index++;
        l->curCol++;
        return lex_tkn(l);

    case ':': return tkn(1, COLON);
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
        switch (*(l->buffer.data + l->index + 1))       \
        {                                               \
        case '=': return tkn(2, TK##_ASGN);             \
        case C: return tkn(1, TK);                      \
        default: return tkn(1, TK##_BIT);               \
        }                                               \

    // #TODO: strtoll for signed integers? or just handle signed ints in parsing
    lex_op('+', ADD)
    lex_op('-', SUB)
    lex_op('*', MUL)
    lex_op('/', DIV)
    lex_op('%', MOD)
    lex_boolop('|', OR)
    lex_boolop('&', AND)

    case EOF:
    case 0:
        return tkn(1, EOF_TOK);

    default:
        // Try lex an identifier or keyword
        if (isalpha(c)) return lex_id_kw(l);
        // Is it a number?
        if (isdigit(c)) return lex_num(l);
    }

    fprintf(stderr, "Error on line %u (column %u): unknown token  %c", l->curLn, l->curCol, c);
    exit(1);
}

#undef lex_op
#undef lex_boolop
#undef tkn

void refill_lex_buf(Lex* l)
{
    while (l->tkWrite != l->tkRead)
    {
        l->tkbuf[l->tkWrite] = lex_tkn(l);
        l->tkWrite = (l->tkWrite + 1) % LEX_BUFFER_SIZE;
    }
}

Lex* lex_start(char* file)
{
    Str data;
    if (!load_file(file, &data))
    {
        fprintf(stderr, "Failed to load file: %s", file);
        return NULL;
    }

    Lex* l = new(Lex);
    *l = (Lex){
        .buffer = data,
        .index = 0,

        .tkbuf = {},
        .tkWrite = 0,
        .tkRead = 0,

        .curLn = 0,
        .curCol = 0,
    };

    return l;
}

void lex_end(Lex* l)
{
    str_free(&l->buffer);
    free(l);
}

Token lex_eat(Lex* l)
{
    refill_lex_buf(l);

    Token tk = l->tkbuf[l->tkRead];
    l->tkRead = (l->tkRead + 1) % LEX_BUFFER_SIZE;

    return tk;
}

Token lex_peek(Lex* l, u8 offset)
{
    assert(offset < LEX_BUFFER_SIZE);

    refill_lex_buf(l);
    return l->tkbuf[(l->tkRead + offset) % LEX_BUFFER_SIZE];
}

void lex_skip(Lex* l, u8 offset)
{
    l->tkRead = (l->tkRead + offset) % LEX_BUFFER_SIZE;
}
