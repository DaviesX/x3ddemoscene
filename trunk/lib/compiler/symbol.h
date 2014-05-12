#ifndef X3DSYMBOL_H_INCLUDED
#define X3DSYMBOL_H_INCLUDED


struct sym_entry {
        int sym_pos;
        int token;
};

void init_keyword_symbol ( struct sym_entry *sym_tbl );
int insert_symbol ( char *sym, struct sym_entry *sym_tbl );
int find_symbol ( char *sym, struct sym_entry *sym_tbl );
char *get_symbol ( int i, struct sym_entry *sym_tbl );


#endif // X3DSYMBOL_H_INCLUDED
