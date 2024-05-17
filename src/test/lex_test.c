// #TODO: this test file should not be included in general build.


#include "../lex.h"
#include "../lib.h"
#include "../ast.h"
#include <assert.h>

void lex_test() {
    Lex* l = lex_start("test/t1.silv");
    Parse* p = parse_begin(l);
    
    while (true) {
        ast_base* ast = parse_one(p);
        if (!ast) break;
        print_expr(ast);
    }

    parse_end(p);
}
