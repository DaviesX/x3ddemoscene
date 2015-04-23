#ifndef SYMBOL_LIB_H_INCLUDED
#define SYMBOL_LIB_H_INCLUDED

#include <container/arraylist.h>
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
bool symlib_load ( struct symbol_set *symbols, char *filename );
bool symlib_unload ( struct symbol_set *symbols );
void symlib_add_abi ( struct symbol_set *symbols, char *name, f_Generic func );
void symlib_add_cstring ( struct symbol_set *symbols, char *name, char *string );
void symlib_add_variable ( struct symbol_set *symbols, char *name, void *value, int size );
f_Generic symlib_ret_abi ( struct symbol_set *symbols, char *sym_name );
void* symlib_ret_variable ( struct symbol_set* symbols, char* sym_name, int* size );
struct dlsymbol* symlib_ret_misc ( struct symbol_set *symbols, char *sym_name );


#endif // SYMBOL_LIB_H_INCLUDED