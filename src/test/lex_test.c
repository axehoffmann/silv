// #TODO: this test file should not be included in general build.


#include "../lex.h"
#include "../lib.h"
#include <assert.h>

void lex_test() {
    Lex* l = lex_start("test/t1.silv");
    Token tk;
    do {
        tk = lex_eat(l);
    } while (tk.type != EOF_TOK);
}
