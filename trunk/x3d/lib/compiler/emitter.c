#include <x3d/common.h>
#include <algorithm.h>
#include "symbol.h"
#include "lexer.h"
#include "emitter.h"


struct postfix_exp {
        struct token_info *o_tok;
        int n_tok;
};


void init_emitter ( struct code_emitter *e )
{
        memset ( e, 0, sizeof ( *e ) );
        e->post_exp = alloc_fix ( sizeof ( struct postfix_exp ), 1 );
        e->post_exp->o_tok = alloc_var ( sizeof ( struct token_info ), 0 );
}

void free_emitter ( struct code_emitter *e )
{
        free_var ( e->post_exp->o_tok );
        free_fix ( e->post_exp );
        memset ( e, 0, sizeof ( *e ) );
}

void emit_postfix ( struct token_info *tok, struct code_emitter *e )
{
        e->post_exp->o_tok = push_var ( tok, e->post_exp->o_tok );
        e->post_exp->n_tok ++;
}

void print_postfix_emitter ( struct code_emitter *e, struct sym_entry *sym_tbl )
{
        struct token_info *tok = e->post_exp->o_tok;
        int i;
        for ( i = 0; i < e->post_exp->n_tok; i ++ ) {
                switch ( tok[i].token ) {
                case TOK_IDR_ADD: {
                        printf ( " + " );
                        break;
                }
                case TOK_IDR_MUL: {
                        printf ( " * " );
                        break;
                }
                case TOK_IDR_NUMBER: {
                        printf ( " %d ", tok[i].tok_attr );
                        break;
                }
                case TOK_IDR_IDENTIFIER: {
                        printf ( " %s ", get_symbol ( tok[i].tok_attr, sym_tbl ) );
                        break;
                }
                }
        }
}
