#ifndef EMITTER_H_INCLUDED
#define EMITTER_H_INCLUDED

struct code_emitter {
        struct postfix_exp *post_exp;
};

struct token_info;

void init_emitter ( struct code_emitter *e );
void free_emitter ( struct code_emitter *e );
void emit_postfix ( struct token_info *tok, struct code_emitter *e );
void print_postfix_emitter ( struct code_emitter *e, struct sym_entry *sym_tbl );


#endif // EMITTER_H_INCLUDED
