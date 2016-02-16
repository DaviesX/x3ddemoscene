#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED


#include <x3d/common.h>
#include <system/symlib.h>

struct alg_var_set;

enum DebugPosition {
        Debug_KernelStart,
        Debug_KernelLoop,
        Debug_KernelHalt,
        DebugPTRendererPrintBenchmarkGeometry,
        DebugPTRendererExportRenderredImage,
        Debug_renderer_c_renderer_render,
        Debug_ptrenderer_c_pt_radiance_node_compute2,
        c_NumDebugPositions
};

typedef void (*f_UT_Init) (struct alg_var_set* envir);
typedef void (*f_UT_Run) (struct alg_var_set* envir);
typedef void (*f_UT_Free) (struct alg_var_set* envir);
typedef enum DebugPosition* (*f_UT_Pos) (struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped);

struct unit_test {
        char*                   test_name;
        f_UT_Init               ut_init;
        f_UT_Run                ut_run;
        f_UT_Free               ut_free;
        f_UT_Pos                ut_pos;
        enum DebugPosition      pos;
        int                     num_run;
        int                     counted;
        clock_t                 begin_time;
        clock_t                 end_time;
        float                   avg_time;
        bool                    is_skipped;
};

bool __public   debugger_init(int argc, char *argv[], struct symbol_set* symbols);
void __public   debugger_free();
bool __private  debugger_invoke_internal(enum DebugPosition pos, struct alg_var_set* params);
#define         debugger_invoke_begin()
#define         debugger_invoke(_pos, _params)          if (debugger_invoke_internal(_pos, _params)) goto _LDB;
#define         debugger_invoke_end()                   _LDB:


#endif // DEBUG_H_INCLUDED
