#ifndef SYMBOL_LIB_H_INCLUDED
#define SYMBOL_LIB_H_INCLUDED

#include <algorithm.h>
#include <x3d/common.h>


enum SYMBOL_IDR {
        SYMBOL_MISC,
        SYMBOL_ABI,
        SYMBOL_CONST_CSTRING,
        SYMBOL_VARIABLE
};

#define cNumSymbolCate          4

/*
 * structures
 */
struct dlsymbol {
        char*                   name;
        f_Generic               func_ptr;
        char*                   const_char;
        void*                   value;
        int                     size;
};

struct symbol_set {
        char*                   filename;
        void*                   handle;
        bool                    is_loaded;
        d_alg_list(dlsymbol)    symbol[cNumSymbolCate];
};

/*
 * functions' declarations
 */
void init_symlib ( struct symbol_set *symbols );
void free_symlib ( struct symbol_set *symbols );
bool symlib_load ( char *filename, struct symbol_set *symbols );
bool symlib_unload ( struct symbol_set *symbols );
void symlib_add_abi ( char *name, f_Generic func, struct symbol_set *symbols );
void symlib_add_cstring ( char *name, char *string, struct symbol_set *symbols );
void symlib_add_variable ( char *name, void *value, int size, struct symbol_set *symbols );
f_Generic symlib_ret_abi ( char *sym_name, struct symbol_set *symbols );
struct dlsymbol* symlib_ret_misc ( char *sym_name, struct symbol_set *symbols );


#endif // SYMBOL_LIB_H_INCLUDED
