#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "symbol.h"
#include "error.h"
#include "emitter.h"
#include "lexer.h"
#include "parser.h"


struct compiler {
        struct string_pool *str;
        struct code_parser parser;
        struct sym_entry *sym_tbl;
};

void init_compiler ( struct compiler *c );
void free_compiler ( struct compiler *c );
int comile_c_to_native ( char *code, struct compiler *c );


#endif // COMPILER_H_INCLUDED
