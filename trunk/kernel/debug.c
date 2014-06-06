#include <x3d/debug.h>



static void init_test_case ( void );
static void finalize_test_case ( void );
static void invoke_test_case ( enum DBG_POSITION pos );
static void gen_debug_info ( void );

static void init_symlib ( void );
static void free_symlib ( void );
static void finalize_symlib ( void );


struct runtime_debug {
        bool finalized;
        struct alg_list test_case;
        struct alg_list case_name;
        struct alg_named_params dbg_info;
};

struct global_symbol {
        bool finalized;
        struct alg_list symbol[3];
};

/* unit test */
static void init_test_case ( void )
{
        struct runtime_debug *dbg = &g_init.rt_debug;
        create_alg_list ( &dbg->test_case, sizeof ( struct unit_test ), 0 );
        create_alg_list ( &dbg->case_name, sizeof ( char* ), 0 );
        int i;
        for ( i = 0; i < g_init.argc - 1; i ++ ) {
                if ( !strcmp ( "--test", g_init.argv[i] ) ) {
                        char *curr = g_init.argv[i + 1];
                        char *casename =
                                alloc_fix ( sizeof casename, strlen ( curr ) + 1 );
                        strcpy ( casename, curr );
                        alg_list_add ( &casename, &dbg->case_name );
                }
        }
        init_named_params ( &dbg->dbg_info );
}

static void finalize_test_case ( void )
{
        g_init.rt_debug.finalized = true;
}

static void invoke_test_case ( enum DBG_POSITION pos )
{
        gen_debug_info ();
        struct runtime_debug *dbg = &g_init.rt_debug;
        char **curr = alg_list_first ( &dbg->case_name );
        if ( pos == DBG_KERNEL_START ) {
                /* do initialization for all test cases */
                int i;
                for ( i = 0; i < alg_list_n ( &dbg->case_name ); i ++ ) {
                        struct unit_test *test;
                        alg_list_i ( &dbg->test_case, i, &test );
                        if ( test->init ) {
                                test->init ( &dbg->dbg_info );
                        }
                }
        } else if ( pos == DBG_KERNEL_HALT ) {
                /* do deconstruction for all test cases */
                int i;
                for ( i = 0; i < alg_list_n ( &dbg->case_name ); i ++ ) {
                        struct unit_test *test;
                        alg_list_i ( &dbg->test_case, i, &test );
                        if ( test->free ) {
                                test->free ( &dbg->dbg_info );
                        }
                }
        }

        struct {
                char *name;
                enum DBG_POSITION pos;
        } info;
        int i;
        for ( i = 0; i < alg_list_n ( &dbg->case_name ); i ++ ) {
                info.name = *curr;
                info.pos = pos;
#define cmp( _info, _elm )	(((struct unit_test *) _elm)->pos & (_info)->pos && \
				 !strcmp ( (_info)->name, ((struct unit_test *) _elm)->test_name ))
                int i_case;
                alg_list_find ( &dbg->test_case, &info, &i_case, cmp );
#undef cmp
                if ( i_case != -1 ) {
                        struct unit_test *test;
                        alg_list_i ( &dbg->test_case, i_case, &test );
                        test->test ( &dbg->dbg_info );
                } else {
                        log_normal_dbg ( "current case: not involve in %x", curr, pos );
                }
                curr ++;
        }
}

static void gen_debug_info ( void )
{
        struct runtime_debug *dbg = &g_init.rt_debug;
        reset_named_params ( &dbg->dbg_info );
        push_named_params ( g_init.argc, "argc", &dbg->dbg_info );
        push_named_params ( g_init.argv, "argv", &dbg->dbg_info );
        push_named_params ( g_init.editor, "editor", &dbg->dbg_info );
        push_named_params ( g_init.scene, "scene", &dbg->dbg_info );
}

void kernel_unit_test_add ( struct unit_test *ut )
{
        if ( g_init.rt_debug.finalized ) {
                log_critical_err_dbg ( "runtime debug has been finalized. error in adding new test case" );
        }
        alg_list_add ( ut, &g_init.rt_debug.test_case );
}

/* symbol library */
static void init_symlib ( void )
{
        memset ( &g_init.sym_lib, 0, sizeof ( struct global_symbol ) );
        g_init.sym_lib.finalized = false;
        int i;
        for ( i = 0; i < 3; i ++ ) {
                create_alg_list ( &g_init.sym_lib.symbol[i],
                                  sizeof ( struct symbol_lib ), 0 );
        }
}

static void free_symlib ( void )
{
        int i;
        for ( i = 0; i < 3; i ++ ) {
                free_alg_list ( &g_init.sym_lib.symbol[i] );
        }
        memset ( &g_init.sym_lib, 0, sizeof ( struct global_symbol ) );
}

static void finalize_symlib ( void )
{
        g_init.sym_lib.finalized = true;
}

void kernel_symlib_add ( struct symbol_lib *symbol )
{
        if ( g_init.sym_lib.finalized ) {
                log_critical_err_dbg ( "symbol library has been finalized. error in adding new symbol" );
        }
        enum SYMBOL_IDR type = symbol->idr;
        alg_list_add ( symbol, &g_init.sym_lib.symbol[type] );
}

bool kernel_symlib_retrieve ( enum SYMBOL_IDR type, char *sym_name,
                              struct symbol_lib *symbol )
{
        struct alg_list *sym_list = &g_init.sym_lib.symbol[type];
        struct {
                char *name;
        } info;
        info.name = sym_name;
#define cmp( _info, _elm )	(!strcmp ( _info->name, \
				 ((struct symbol_lib *) (_elm))->sym_name ))
        int i_sym;
        alg_list_find ( sym_list, &info, &i_sym, cmp );
#undef cmp
        if ( i_sym != -1 ) {
                struct symbol_lib *sym_found;
                alg_list_i ( sym_list, i_sym, &sym_found );
                memcpy ( symbol, sym_found, sizeof *sym_found );
                return true;
        } else {
                return false;
        }
}
