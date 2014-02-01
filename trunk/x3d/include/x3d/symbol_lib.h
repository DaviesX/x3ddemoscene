#ifndef SYMBOL_LIB_H_INCLUDED
#define SYMBOL_LIB_H_INCLUDED

#include <x3d/common.h>

enum SYMBOL_IDR {
	SYMLIB_FUNC,
	SYMLIB_VAR,
	SYMLIB_TYPE,
};

/*
 * structures
 */
struct symbol_lib {
	char *sym_name;
	enum SYMBOL_IDR idr;
	char *sym_defn;
	uint64_t addr;		/* pointers may have different length */
};

/*
 * functions' declarations
 */
void kernel_symlib_add ( struct symbol_lib *symbol );
bool kernel_symlib_retrieve ( enum SYMBOL_IDR type, char *sym_name,
			      struct symbol_lib *symbol );


#endif // SYMBOL_LIB_H_INCLUDED
