// #TODO: this test file should not be included in general build.


#include "../lex.h"
#include "../lib.h"
#include "../ast.h"
#include <assert.h>

void lex_test() {
    Lex* l = lex_start("test/t1.silv");
    Parse* p = parse_begin(l);
    
    print_expr(parse_one(p));

    parse_end(p);
}
