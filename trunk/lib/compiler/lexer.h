#ifndef X3DCOMPILERLEXER_H_INCLUDED
#define X3DCOMPILERLEXER_H_INCLUDED


enum TOKEN_IDR {
        TOK_IDR_NULL,
        TOK_IDR_NUMBER,
        TOK_IDR_IDENTIFIER,
        TOK_IDR_END_STAT,
        TOK_IDR_BEGIN_BLOCK,
        TOK_IDR_END_BLOCK,
        TOK_IDR_BEGIN_PARENT,
        TOK_IDR_END_PARENT,
        TOK_IDR_ADD,
        TOK_IDR_SUB,
        TOK_IDR_MUL,
        TOK_IDR_DIV,
        TOK_IDR_MOD,
        TOK_IDR_ASSIGN,
        TOK_IDR_BIT_OR,
        TOK_IDR_BIT_AND,
        TOK_IDR_BIT_XOR,
        TOK_IDR_BIT_INV,
        TOK_IDR_BIT_LSH,
        TOK_IDR_BIT_RSH,
        TOK_IDR_LOG_OR,
        TOK_IDR_LOG_AND,
        TOK_IDR_LOG_INV,
        TOK_IDR_LOG_EQL,
        TOK_IDR_LOG_LEQ,
        TOK_IDR_LOG_GEQ,
        TOK_IDR_LOG_NEQ,
};

struct token_info {
        enum TOKEN_IDR token;
        int tok_attr;
};

struct op_info {
        int prio;
        int asso;
};

struct code_lexer {
        int iline;
        struct token_info tok_info;
        char *tok_buf;
        char *code;
        char *ptr;
};

struct sym_entry;


#define TOKEN_LEFT_ASSOCIATE		1
#define TOKEN_RIGHT_ASSOCIATE		2

void init_lexer ( char *code, struct code_lexer *lex );
void free_lexer ( struct code_lexer *lex );
void lex_code ( struct code_lexer *lex, struct sym_entry *sym_tbl );
void get_op_info_lexer ( struct token_info *tok_info, struct op_info *op );
void get_token_info_lexer ( struct code_lexer *lex, struct token_info *tok_info );


#endif // X3DCOMPILERLEXER_H_INCLUDED
