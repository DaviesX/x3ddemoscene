#include <misc.h>
#include <container/paramset.h>
#include <system/allocator.h>
#include <system/symlib.h>
#include <system/log.h>
#include <x3d/debug.h>


struct runtime_debug {
        bool                    finalized;
        int                     argc;
        char**                  argv;
        d_alg_list(unit_test)*  test_case;
        struct alg_var_set      dbg_env;
};

struct runtime_debug g_rtdebug;


/* unit test */
bool debugger_init(int argc, char** argv, struct symbol_set *symbols)
{
        struct runtime_debug *dbg = &g_rtdebug;
        dbg->argc = argc;
        dbg->argv = argv;
        alg_var_set_init(&dbg->dbg_env);
        alg_var_set_declare(&dbg->dbg_env, "argc", &argc, sizeof(argc));
        alg_var_set_declare(&dbg->dbg_env, "argv", &argv, sizeof(argv));
        alg_var_set_declare(&dbg->dbg_env, "x3d-core-symbol", symbols, sizeof(*symbols));
        dbg->test_case = alloc_fix(sizeof(*dbg->test_case), c_NumDebugPositions);
        int n;
        for (n = 0; n < c_NumDebugPositions; n ++) {
                alg_init(list, &dbg->test_case[n], sizeof(struct unit_test), 0);
        }
        int n_matched;
        char** test_cases = misc_get_command_params("--test", argc, argv, &n_matched);
        int i;
        for (i = 0; i < n_matched; i ++) {
                char* test = test_cases[i];

                char test_func[64];
                char test_init[64];
                char test_free[64];
                char test_pos[64];
                strcpy(test_func, test);

                strcpy(test_init, test);
                strcat(test_init, "_init");

                strcpy(test_free, test);
                strcat(test_free, "_free");

                strcpy(test_pos, test);
                strcat(test_pos, "_pos");

                f_UT_Init ut_init      = (f_UT_Init) symlib_ret_abi(symbols, test_init);
                f_UT_Run  ut_run       = (f_UT_Run)  symlib_ret_abi(symbols, test_func);
                f_UT_Free ut_free      = (f_UT_Free) symlib_ret_abi(symbols, test_free);
                f_UT_Pos  ut_pos       = (f_UT_Pos)  symlib_ret_abi(symbols, test_pos);
                if (!ut_init || !ut_run || !ut_free || !ut_pos) {
                        log_severe_err_dbg("couldn't find symbols for test case: %s", test);
                        return false;
                }
                enum DebugPosition*     pos_list;
                int                     n_pos;
                int                     num_run;
                bool                    is_skipped;
                if (!(pos_list = ut_pos(&dbg->dbg_env, &n_pos, &num_run, &is_skipped))) {
                        log_severe_err_dbg("test case: %s, failed to install debug position", test);
                        return false;
                } else {
                        int j;
                        for (j = 0; j < n_pos; j ++) {
                                struct unit_test ut;
                                ut.test_name    = alg_alloc_string(test);
                                ut.ut_init      = ut_init;
                                ut.ut_run       = ut_run;
                                ut.ut_free      = ut_free;
                                ut.ut_pos       = ut_pos;
                                ut.pos          = pos_list[j];
                                ut.num_run      = num_run;
                                ut.counted      = 0;
                                ut.is_skipped   = is_skipped;
                                ut.begin_time   = 0;
                                ut.end_time     = 0;
                                ut.avg_time     = 0;
                                if (pos_list[j] < c_NumDebugPositions) {
                                        alg_push_back(list, &dbg->test_case[pos_list[j]], ut);
                                } else {
                                        log_severe_err_dbg("invalid position speciifed by: %s", test_pos);
                                        return false;
                                }
                        }

                }
        }
        free_fix(test_cases);
        return true;
}

void debugger_free()
{
        struct runtime_debug *dbg = &g_rtdebug;
        int i;
        for (i = 0; i < c_NumDebugPositions; i ++) {
                struct unit_test* tests = alg_array(list, &dbg->test_case[i]);
                int j;
                for (j = 0; j < alg_n(list, &dbg->test_case[i]); j ++) {
                        struct unit_test* ut = &tests[j];
                        free_fix(ut->test_name);
                        zero_obj(ut);
                }
        }
        free_fix(dbg->test_case);
        alg_var_set_free(&dbg->dbg_env);
        zero_obj(dbg);
}

bool debugger_invoke_internal(enum DebugPosition pos, struct alg_var_set* params)
{
        struct runtime_debug *dbg = &g_rtdebug;
        struct unit_test* tests = alg_array(list, &dbg->test_case[pos]);
        int i;
        for (i = 0; i < alg_n(list, &dbg->test_case[pos]); i ++) {
                struct unit_test* ut = &tests[i];
                if (ut->counted == 0) {
                        ut->ut_init(params);
                }
                if (ut->counted ++ < ut->num_run) {
                        ut->begin_time = clock();
                        ut->ut_run(params);
                        ut->end_time = clock();
                        float time_used = (float) (ut->end_time - ut->begin_time)/CLOCKS_PER_SEC;
                        ut->avg_time += time_used;
                        if (ut->num_run != INT_MAX) {
                                log_normal("%s#%d ====> %fsec", ut->test_name, ut->counted, time_used);
                        }
                }
                if (ut->counted == ut->num_run) {
                        ut->ut_free(params);
                        ut->avg_time /= ut->num_run;
                        log_normal("%s#avg ====>%fsec", ut->test_name, ut->avg_time);
                }
                if (ut->is_skipped) {
                        return true;
                }
        }
        return false;
}
