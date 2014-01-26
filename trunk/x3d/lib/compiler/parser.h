#ifndef X3DCOMPILERPARSER_H_INCLUDED
#define X3DCOMPILERPARSER_H_INCLUDED


struct code_parser {
	struct code_lexer lexer;
	struct code_emitter em;
	struct internal_data *data;
};

void init_parser ( struct code_parser *parser );
void free_parser ( struct code_parser *parser );
void parse_code ( char *code, struct code_parser *parser, struct sym_entry *sym_tbl );
void get_emitter_parser ( struct code_parser *parser, struct code_emitter *em );


#endif // X3DCOMPILERPARSER_H_INCLUDED
