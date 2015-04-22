#include <x3d/common.h>
#include <container/paramset.h>
#include <compiler/compiler.h>


int c_compiler_test ( struct alg_var_set* envir )
{
        char *code = "8+ 3*5 + 6*7;";
        printf ( "code: %s\n", code );
        struct compiler c;
        compiler_init ( &c );
//        compiler_lex ( code, &c );
        compiler_free ( &c );
        return 0;
}
