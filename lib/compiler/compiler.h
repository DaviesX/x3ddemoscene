#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "symbol.h"
#include "error.h"
#include "emitter.h"
#include "lexer.h"
#include "parser.h"


enum CC_LANG_IDR {
        CC_LANG_C,
        CC_LANG_GLSL,
        CC_LANG_X3DSHADER,
        CC_LANG_CUSTOM
};

struct compiler {
        struct string_pool *str;
        struct code_parser parser;
        struct sym_entry *sym_tbl;
};

struct module {
};

/*
 * functions' declartion
 */
void init_compiler ( void );
void free_compiler ( void );
void compiler_init ( struct compiler* c );
void compiler_free ( struct compiler* c );
bool compiler_use_language ( struct compiler* c, enum CC_LANG_IDR type, char* custom_syn );
bool compiler_lex ( struct compiler* c, struct module* m );
bool compiler_parse ( struct compiler* c, struct module* m );
bool compiler_link_to_x86 ( struct compiler *c, struct module* m );


#endif // COMPILER_H_INCLUDED
