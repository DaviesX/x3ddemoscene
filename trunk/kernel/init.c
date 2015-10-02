#include <misc.h>
#include <x3d/common.h>
#include <math/math.h>
#include <system/thread.h>
#include <system/log.h>
#include <system/symlib.h>
#include <container/paramset.h>
#include <x3d/init.h>
#include <x3d/debug.h>
#include <x3d/resloader.h>
#include <x3d/renderer.h>


struct signal {
        f_On_Init               on_init;
        f_On_Rest_Init          on_rest_init;
        f_On_Loop_Init          on_loop_init;
        f_On_Loop               on_loop;
        f_On_Loop_Free          on_loop_free;
        f_On_Free               on_free;
        void*                   env;
};

struct init {
        int                     argc;
        char**                  argv;
        bool                    is_init;
        bool                    to_run;
        struct signal           signal;
        struct symbol_set       symbols;
};

static struct init g_init = {0};

static void rest_init();
static void kernel_init_param(int argc, char** argv);
static void kernel_free_param();


static void kernel_init_param(int argc, char** argv)
{
        g_init.argc = argc;
        g_init.argv = alloc_var(sizeof(char*), argc);
        g_init.argv = alloc_expand_var(g_init.argv, argc);
        int i;
        for (i = 0; i < argc; i ++) {
                g_init.argv[i] = alg_alloc_string(argv[i]);
        }
}

static void kernel_free_param()
{
        int i;
        for (i = 0; i < g_init.argc; i ++) {
                free_fix(g_init.argv[i]);
        }
        free_var(g_init.argv);
        g_init.argc = 0;
}

static char* get_self_so ( int argc, char* argv[] )
{
        int i;
        for ( i = 0; i < argc; i ++ ) {
                if ( !strcmp ( "--so_path", argv[i] ) ) {
                        if ( i + 1 < argc )
                                return argv[i + 1];
                        else
                                goto retry;
                }
        }
retry:
        return "./libX3dCore.so";
}

__dlexport int main(int argc, char* argv[])
{
        kernel_init_param(argc, argv);
        int e = kernel_main ( argc, argv );
        kernel_free_param();
        return e;
}

__dlexport bool kernel_start ( void )
{
        struct init*   init = &g_init;
        struct signal* signal = &init->signal;

        thread_lib_init();
        if (signal->on_init != nullptr) {
                signal->on_init(init->argc, init->argv, signal->env);
        }

        if (!log_init(true)) {
                return false;
        }
        symlib_init(&init->symbols);
        if (!symlib_load(&init->symbols, get_self_so(init->argc, init->argv))) {
                return false;
        }
        if (!debugger_init(init->argc, init->argv, &init->symbols)) {
                return false;
        }
        math_lib_init();
        if (!renderer_import(&init->symbols)) {
                return false;
        }
        renderer_kernel_init();

        struct alg_var_set params;
        alg_var_set_init(&params);
        debugger_invoke_begin();
                debugger_invoke(Debug_KernelStart, &params);

        rest_init ();

        debugger_invoke_end();
        alg_var_set_free(&params);
        return true;
}

__dlexport void kernel_halt()
{
        g_init.to_run = false;
}

static void rest_init()
{
        struct init*   init     = &g_init;
        struct signal* signal   = &init->signal;

        if (signal->on_rest_init != nullptr) {
                signal->on_rest_init(signal->env);
        }
        g_init.to_run = true;
}

__dlexport void kernel_loop()
{
        struct init*   init = &g_init;
        struct signal* signal = &init->signal;

        if (signal->on_loop_init != nullptr)
                signal->on_loop_init(signal->env);

        struct alg_var_set params;
        alg_var_set_init(&params);
        while (g_init.to_run) {
                debugger_invoke_begin();
                debugger_invoke(Debug_KernelLoop, &params);

                if (signal->on_loop != nullptr)
                        signal->on_loop(signal->env);
        }

        if (signal->on_loop_free != nullptr)
                signal->on_loop_free(signal->env);

        debugger_invoke(Debug_KernelHalt, &params);

        debugger_invoke_end();
        alg_var_set_free(&params);
}

__dlexport void kernel_shutdown ( void )
{
        struct init*   init = &g_init;
        struct signal* signal = &init->signal;

        if (!init->to_run) {
                log_mild_err_dbg("the kernel is not launched. so it will not shutdown.");
                log_free();
                return ;
        }
        if (signal->on_free != nullptr) {
                signal->on_free(signal->env);
        }
        symlib_free(&init->symbols);
        thread_lib_free();
        log_free();
}

__dlexport bool kernel_is_running ( void )
{
        return g_init.to_run;
}

__dlexport void kernel_add_param(const char *option, const char *param)
{
        g_init.argv = alloc_add_var(g_init.argv, 2);
        int n = g_init.argc;
        g_init.argv[n + 0] = alg_alloc_string(option);
        g_init.argv[n + 1] = alg_alloc_string(param);
        g_init.argc += 2;
}

__dlexport void kernel_reg_environment ( void* env )
{
        g_init.signal.env = env;
}

__dlexport void kernel_reg_signal ( char* name, f_Generic sig_func )
{
        struct init*   init = &g_init;
        struct signal* signal = &init->signal;

        if ( !strcmp ( name, "on-init" ) )
                signal->on_init = (f_On_Init) sig_func;
        else if ( !strcmp ( name, "on-rest-init" ) )
                signal->on_rest_init = (f_On_Rest_Init) sig_func;
        else if ( !strcmp ( name, "on-loop-init" ) )
                signal->on_loop_init = (f_On_Loop_Init) sig_func;
        else if ( !strcmp ( name, "on-loop" ) )
                signal->on_loop = (f_On_Loop) sig_func;
        else if ( !strcmp ( name, "on-loop-free" ) )
                signal->on_loop_free = (f_On_Loop_Free) sig_func;
        else if ( !strcmp ( name, "on-free" ) )
                signal->on_free = (f_On_Free) sig_func;
        else
                log_mild_err_dbg ( "unknown signal type: %s", name );
}

__dlexport void kernel_ax_signal ( char *name )
{
        struct init*   init = &g_init;
        struct signal* signal = &init->signal;

        if ( !strcmp ( name, "on-init" ) )
                signal->on_init = nullptr;
        else if ( !strcmp ( name, "on-rest-init" ) )
                signal->on_rest_init = nullptr;
        else if ( !strcmp ( name, "on-loop-init" ) )
                signal->on_loop_init = nullptr;
        else if ( !strcmp ( name, "on-loop" ) )
                signal->on_loop = nullptr;
        else if ( !strcmp ( name, "on-loop-free" ) )
                signal->on_loop_free = nullptr;
        else if ( !strcmp ( name, "on-free" ) )
                signal->on_free = nullptr;
        else
                log_mild_err_dbg ( "unknown signal type: %s", name );
}
