#include <x3d/common.h>
#include <system/allocator.h>
#include "symbol.h"
#include "lexer.h"


#define EOT		0	/* End of token */

const struct op_info OpInfo[] = {
        [TOK_IDR_NULL].prio = -1,
        [TOK_IDR_NULL].asso = -1,
        [TOK_IDR_ADD].prio = TOK_IDR_ADD,
        [TOK_IDR_ADD].asso = TOKEN_LEFT_ASSOCIATE,
        [TOK_IDR_MUL].prio = TOK_IDR_MUL,
        [TOK_IDR_MUL].asso = TOKEN_LEFT_ASSOCIATE
};

void trans_op ( char cch, struct token_info *tok_info );

void init_lexer ( char *code, struct code_lexer *lex )
{
        memset ( lex, 0, sizeof ( *lex ) );
        lex->tok_buf = alloc_var ( sizeof ( char ), 0 );
        lex->code = code;
        lex->ptr = code;
}

void free_lexer ( struct code_lexer *lex )
{
        free_var ( lex->tok_buf );
        memset ( lex, 0, sizeof ( *lex ) );
}

void trans_op ( char cch, struct token_info *tok_info )
{
        switch ( cch ) {	/** FIXME: State changing */
        case '+': {
                tok_info->token = TOK_IDR_ADD;
                tok_info->tok_attr = TOK_IDR_NULL;
                break;
        }
        case '*': {
                tok_info->token = TOK_IDR_MUL;
                tok_info->tok_attr = TOK_IDR_NULL;
                break;
        }
        case ';': {
                tok_info->token = TOK_IDR_END_STAT;
                tok_info->tok_attr = TOK_IDR_NULL;
                break;
        }
        }
}

void lex_code ( struct code_lexer *lex, struct sym_entry *sym_tbl )
{
        while ( 1 ) {
                char cch = *lex->ptr;
                if ( cch == ' ' || cch == '\t' ) {
                        lex->ptr ++;
                        continue;
                } else if ( cch == '\n' ) {
                        lex->iline ++;
                        lex->ptr ++;
                        continue;
                } else if ( isdigit ( cch ) ) {
                        int attr;
                        sscanf ( lex->ptr, "%d", &attr );	/** FIXME: get_length **/
                        lex->ptr ++;
                        lex->tok_info.token = TOK_IDR_NUMBER;
                        lex->tok_info.tok_attr = attr;
                        break;
                } else if ( isalpha ( cch ) ) {
                        int b = 0;
                        while ( isalnum ( lex->ptr[b] ) ) {
                                lex->tok_buf = add_var ( lex->tok_buf, 1 );
                                lex->tok_buf[b] = lex->ptr[b];
                                b ++;
                        }
                        lex->tok_buf[b] = EOT;
                        lex->ptr += b;

                        int p = find_symbol ( lex->tok_buf, sym_tbl );
                        if ( p == -1 ) {
                                p = insert_symbol ( lex->tok_buf, sym_tbl );
                        }
                        flush_var ( lex->tok_buf );

                        lex->tok_info.token = TOK_IDR_IDENTIFIER;
                        lex->tok_info.tok_attr = p;
                        break;
                } else if ( cch == 0 ) {
                        lex->tok_info.token = TOK_IDR_NULL;
                        lex->tok_info.tok_attr = TOK_IDR_NULL;
                        break;
                } else {
                        lex->ptr ++;
                        trans_op ( cch, &lex->tok_info );
                        break;
                }
        }
}

void get_op_info_lexer ( struct token_info *tok_info, struct op_info *op )
{
        memcpy ( op, &OpInfo[tok_info->token], sizeof ( *op ) );
}

void get_token_info_lexer ( struct code_lexer *lex, struct token_info *tok_info )
{
        memcpy ( tok_info, &lex->tok_info, sizeof ( *tok_info ) );
}
