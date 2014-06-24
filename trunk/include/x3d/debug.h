#ifndef RUNTIME_DEBUG_H_INCLUDED
#define RUNTIME_DEBUG_H_INCLUDED


#include <x3d/common.h>
#include <symlib.h>

struct alg_var_set;

enum DBG_POSITION {
        DBG_KERNEL_START 	= 0X1,
        DBG_KERNEL_LOOP_ONCE 	= 0X2,
        DBG_KERNEL_LOOP 	= 0X4,
        DBG_KERNEL_HALT 	= 0X8
};
#define cNumDbgPosition         4

typedef void (*f_UT_Init) ( struct alg_var_set* envir );
typedef void (*f_UT_Run) ( struct alg_var_set* envir );
typedef void (*f_UT_Free) ( struct alg_var_set* envir );
typedef enum DBG_POSITION (*f_UT_Pos) ( struct alg_var_set* envir );

struct unit_test {
        char*                   test_name;
        f_UT_Init               ut_init;
        f_UT_Run                ut_run;
        f_UT_Free               ut_free;
        f_UT_Pos                ut_pos;
        enum DBG_POSITION       pos;
};

void init_debugger ( int argc, char *argv[], struct symbol_set* symbols );
void debugger_invoke ( enum DBG_POSITION pos );


#endif // RUNTIME_DEBUG_H_INCLUDED
