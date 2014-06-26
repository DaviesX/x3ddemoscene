#include <algorithm.h>
#include <symlib.h>
#include <logout.h>
#include <x3d/debug.h>


struct runtime_debug {
        bool                    finalized;
        int                     argc;
        char**                  argv;
        d_alg_list(unit_test)   test_case[cNumDbgPosition];
        struct alg_var_set      dbg_env;
};

struct runtime_debug g_rtdebug;


/* unit test */
bool init_debugger ( int argc, char *argv[], struct symbol_set *symbols )
{
        struct runtime_debug *dbg = &g_rtdebug;
        dbg->argc = argc;
        dbg->argv = argv;
        init_alg_var_set ( &dbg->dbg_env );
        alg_var_set_declare ( "argc", &argc, sizeof(argc), &dbg->dbg_env );
        alg_var_set_declare ( "argv", &argv, sizeof(argv), &dbg->dbg_env );
        alg_var_set_declare ( "x3d-core-symbol", symbols, sizeof(*symbols), &dbg->dbg_env );

        int n;
        for ( n = 0; n < cNumDbgPosition; n ++ )
                alg_init ( list, sizeof(struct unit_test), 0,
                           &dbg->test_case[n] );
        int i;
        for ( i = 0; i < argc - 1; i ++ ) {
                if ( !strcmp ( "--test", argv[i] ) ) {
                        char* test = argv[i + 1];

                        char test_func[64];
                        char test_init[64];
                        char test_free[64];
                        char test_pos[64];
                        strcpy ( test_func, test );

                        strcpy ( test_init, test );
                        strcat ( test_init, "_init" );

                        strcpy ( test_free, test );
                        strcat ( test_free, "_free" );

                        strcpy ( test_pos, test );
                        strcat ( test_pos, "_pos" );

                        struct unit_test ut;
                        ut.test_name    = alg_alloc_string ( test );
                        ut.ut_init      = (f_UT_Init) symlib_ret_abi ( test_init, symbols );
                        ut.ut_run       = (f_UT_Run)  symlib_ret_abi ( test_func, symbols );
                        ut.ut_free      = (f_UT_Free) symlib_ret_abi ( test_free, symbols );
                        ut.ut_pos       = (f_UT_Pos)  symlib_ret_abi ( test_pos, symbols );
                        if ( !ut.ut_init || !ut.ut_run ||
                             !ut.ut_free || !ut.ut_pos ) {
                                log_severe_err_dbg ( "couldn't find symbols" );
                                return false;
                        }
                        ut.pos          = ut.ut_pos ( &dbg->dbg_env );
                        if ( ut.pos & DBG_KERNEL_START ) {
                                alg_push_back ( list, ut, &dbg->test_case[DBG_INDEX_START] );
                        } else if ( ut.pos & DBG_KERNEL_LOOP_ONCE ) {
                                alg_push_back ( list, ut, &dbg->test_case[DBG_INDEX_LOOP_ONCE] );
                        } else if ( ut.pos & DBG_KERNEL_LOOP ) {
                                alg_push_back ( list, ut, &dbg->test_case[DBG_INDEX_LOOP] );
                        } else if ( ut.pos & DBG_KERNEL_HALT ) {
                                alg_push_back ( list, ut, &dbg->test_case[DBG_INDEX_HALT] );
                        } else {
                                log_severe_err_dbg ( "invalid position speciifed by: %s", test_pos );
                                return false;
                        }
                }
        }
        return true;
}

void debugger_invoke ( enum DBG_INDEX pos )
{
        struct runtime_debug *dbg = &g_rtdebug;

        switch ( pos ) {
        case DBG_INDEX_START: {
                /* do initialization for all test cases */
                int n;
                for ( n = 0; n < cNumDbgPosition; n ++ ) {
                        struct unit_test* tests = alg_array ( list, &dbg->test_case[n] );
                        int i;
                        for ( i = 0; i < alg_n ( list, &dbg->test_case[n] ); i ++ ) {
                                struct unit_test* curr = &tests[i];
                                if ( curr->ut_init ) {
                                        curr->ut_init ( &dbg->dbg_env );
                                }
                        }
                }
                /* run test case of kernel start */
                struct unit_test* tests = alg_array ( list, &dbg->test_case[DBG_INDEX_START] );
                int i;
                for ( i = 0; i < alg_n ( list, &dbg->test_case[DBG_INDEX_START] ); i ++ ) {
                        struct unit_test* curr = &tests[i];
                        if ( curr->ut_run ) {
                                curr->ut_run ( &dbg->dbg_env );
                        }
                }
                break;
        }
        case DBG_INDEX_HALT: {
                /* run test case of kernel halt */
                struct unit_test* tests = alg_array ( list, &dbg->test_case[DBG_INDEX_HALT] );
                int i;
                for ( i = 0; i < alg_n ( list, &dbg->test_case[DBG_INDEX_HALT] ); i ++ ) {
                        struct unit_test* curr = &tests[i];
                        if ( curr->ut_run ) {
                                curr->ut_run ( &dbg->dbg_env );
                        }
                }
                /* do deconstruction for all test cases */
                int n;
                for ( n = 0; n < cNumDbgPosition; n ++ ) {
                        struct unit_test* tests = alg_array ( list, &dbg->test_case[n] );
                        int i;
                        for ( i = 0; i < alg_n ( list, &dbg->test_case[n] ); i ++ ) {
                                struct unit_test* curr = &tests[i];
                                if ( curr->ut_free ) {
                                        curr->ut_free ( &dbg->dbg_env );
                                }
                        }
                }
                break;
        }
        case DBG_INDEX_LOOP: {
                struct unit_test* tests = alg_array ( list, &dbg->test_case[DBG_INDEX_LOOP] );
                int num_test = alg_n ( list, &dbg->test_case[DBG_INDEX_LOOP] );
                int i;
                for ( i = 0; i < num_test; i ++ ) {
                        struct unit_test* curr = &tests[i];
                        if ( curr->ut_run ) {
                                curr->ut_run ( &dbg->dbg_env );
                        }
                }
                break;
        }
        case DBG_INDEX_LOOP_ONCE: {
                static bool first_time = true;
                if ( first_time == false )
                        break;
                else
                        first_time = false;

                struct unit_test* tests = alg_array ( list, &dbg->test_case[DBG_INDEX_LOOP_ONCE] );
                int num_test = alg_n ( list, &dbg->test_case[DBG_INDEX_LOOP_ONCE] );
                int i;
                for ( i = 0; i < num_test; i ++ ) {
                        struct unit_test* curr = &tests[i];
                        if ( curr->ut_run ) {
                                curr->ut_run ( &dbg->dbg_env );
                        }
                }
                break;
        }
        }
}
