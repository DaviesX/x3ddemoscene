#include <x3d/common.h>
#include <algorithm.h>
#include <compiler/compiler.h>


int c_compiler_test ( struct alg_var_set* envir )
{
        char *code = "8+ 3*5 + 6*7;";
        printf ( "code: %s\n", code );
        struct compiler c;
        init_compiler ( &c );
        comile_c_to_native ( code, &c );
        free_compiler ( &c );
        return 0;
}
