#include <system/allocator.h>
#include <compiler/compiler.h>


void init_compiler ( void )
{
}

void free_compiler ( void )
{
}

void compiler_init ( struct compiler* c )
{
}

void compiler_free ( struct compiler* c )
{
}

bool compiler_use_language ( struct compiler* c, enum CC_LANG_IDR type, char* custom_syn )
{
        return false;
}

bool compiler_lex ( struct compiler* c, struct module* m )
{
        return false;
}

bool compiler_parse ( struct compiler* c, struct module* m )
{
        return false;
}

bool compiler_link_to_x86 ( struct compiler *c, struct module* m )
{
        return false;
}

/*
void init_compiler ( struct compiler *c )
{
        c->sym_tbl = alloc_var ( sizeof ( struct sym_entry ), 0 );
        init_keyword_symbol ( c->sym_tbl );
        init_parser ( &c->parser );
}

void free_compiler ( struct compiler *c )
{
        free_parser ( &c->parser );
}

int comile_c_to_native ( char *code, struct compiler *c )
{
        parse_code ( code, &c->parser, c->sym_tbl );
        struct code_emitter em;
        get_emitter_parser ( &c->parser, &em );
        print_postfix_emitter ( &em, c->sym_tbl );
        return 1;
}
*/
