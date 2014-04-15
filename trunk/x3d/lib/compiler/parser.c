#include <x3d/common.h>
#include <algorithm.h>
#include "symbol.h"
#include "error.h"
#include "emitter.h"
#include "lexer.h"
#include "parser.h"


struct internal_data {
        struct token_info next_tok;
};

void parse_exp ( struct code_parser *parser, struct sym_entry *sym_tbl );

void init_parser ( struct code_parser *parser )
{
        memset ( parser, 0, sizeof ( *parser ) );
        parser->data = alloc_fix ( sizeof ( struct internal_data ), 1 );
        init_emitter ( &parser->em );
}

void free_parser ( struct code_parser *parser )
{
        free_fix ( parser->data );
        memset ( parser, 0, sizeof ( *parser ) );
}

void parse_code ( char *code, struct code_parser *parser, struct sym_entry *sym_tbl )
{
        init_lexer ( code, &parser->lexer );
        lex_code ( &parser->lexer, sym_tbl );
        get_token_info_lexer ( &parser->lexer, &parser->data->next_tok );
        while ( parser->data->next_tok.token != TOK_IDR_NULL ) {
                parse_exp ( parser, sym_tbl );
                get_token_info_lexer ( &parser->lexer, &parser->data->next_tok );
                if ( parser->data->next_tok.token == TOK_IDR_END_STAT ) {
                        lex_code ( &parser->lexer, sym_tbl );
                        get_token_info_lexer ( &parser->lexer, &parser->data->next_tok );
                } else {
                        /* There is an error */
                        return ;
                }
        }
        free_lexer ( &parser->lexer );
}

void parse_exp ( struct code_parser *parser, struct sym_entry *sym_tbl )
{
        declare_stack ( opr_stack, 128 );
        init_stack ( &opr_stack );
        struct internal_data *data = parser->data;

        while ( 1 ) {
                switch ( data->next_tok.token ) {
                case TOK_IDR_NUMBER: {
                        emit_postfix ( &data->next_tok, &parser->em );
                        break;
                }
                case TOK_IDR_IDENTIFIER: {
                        emit_postfix ( &data->next_tok, &parser->em );
                        break;
                }
                case TOK_IDR_ADD:
                case TOK_IDR_SUB:
                case TOK_IDR_MUL:
                case TOK_IDR_DIV: {
                        if ( check_stack ( &opr_stack ) ) {
                                struct op_info top_opr;
                                struct op_info curr_opr;
                                struct token_info top_tok;
                                get_stack_last ( &opr_stack, top_tok );
                                get_op_info_lexer ( &data->next_tok, &curr_opr );
                                get_op_info_lexer ( &top_tok, &top_opr );
                                if ( curr_opr.prio <= top_opr.prio ) {
                                        pop_stack ( &opr_stack, top_tok );
                                        emit_postfix ( &top_tok, &parser->em );
                                }
                        }
                        push_stack ( &opr_stack, data->next_tok );
                        break;
                }
                case TOK_IDR_BEGIN_PARENT: {
                        break;
                }
                case TOK_IDR_END_STAT: {
                        while ( check_stack ( &opr_stack ) ) {
                                struct token_info top_tok;
                                pop_stack ( &opr_stack, top_tok );
                                emit_postfix ( &top_tok, &parser->em );
                        }
                        return ;
                }
                default: {
                        return ;
                }
                }
                lex_code ( &parser->lexer, sym_tbl );
                get_token_info_lexer ( &parser->lexer, &data->next_tok );
        }
}

void get_emitter_parser ( struct code_parser *parser, struct code_emitter *em )
{
        memcpy ( em, &parser->em, sizeof ( *em ) );
}
/*
void get_term ( void )
{
	int t;
	factor ();
	while ( 1 ) {
		switch ( lookahead ) {
		case '*':
		case '/':
		case DIV:
		case MOD: {
			t = lookahead;
			match ( lookahead );
			factor ();
			emit ( t, NONE );
			continue;
		}
		default: {
			return ;
		}
		}
	}
}

void factor ( void )
{
	switch ( lookahead ) {
	case '(': {
		match ( '(' );
		expr ();
		match ( ')' );
		break;
	}
	case NUM: {
		emit ( NUM, tokenval );
		match ( NUM );
		break;
	}
	case ID: {
		emit ( ID, tokenval );
		match ( ID );
		break;
	}
	default: {
		error ( "syntax error" );
	}
	}
}

void match ( int t )
{
	if ( lookahead == t ) {
		lookahead = lexan ();
	} else {
		error ( "syntax error" );
	}
}
*/

