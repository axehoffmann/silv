// #TODO: this test file should not be included in general build.


#include "../lex.h"
#include "../lib.h"
#include <assert.h>

void lex_test() {
    Lex* l = lex_start("test/t1.silv");
    Token tk;
    u32 ln = 0;
    do {
        tk = lex_eat(l);

        if (ln != tk.line) puts("\n");
        if (tk.type == IDENT) {
            printf("(%u, %.*s) ", tk.str.size, tk.str.size, tk.str.data);
        } else
            printf("%i ", tk.type);


        ln = tk.line;
        
    } while (tk.type != EOF_TOK);

    printf("\nEND\n");
}
