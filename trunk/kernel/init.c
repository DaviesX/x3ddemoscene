#include <x3d/common.h>
#include <math/math.h>
#include <algorithm.h>
#include <thread.h>
#include <logout.h>
#include <lib.h>
#include <x3d/init.h>
#include <x3d/debug.h>
#include <x3d/symbol_lib.h>
#include <x3d/resloader.h>
#include <x3d/renderer.h>
#include <x3d/world.h>


struct signal {
        f_On_Init               on_init;
        f_On_Rest_Init          on_rest_init;
};

struct init {
        int argc;
        char **argv;
        bool is_init;
        bool to_run;
        struct signal signal;
};

static struct init g_init = {0};

static void rest_init ( void );
static void kernel_loop ( void );


__dlexport void kernel_start ( void )
{
        g_init.is_init = true;
        init_test_case ();
        init_symlib ();
        init_math_lib ();
        init_log_output ( true );
        init_thread_lib ();
        init_lib ();
//        init_res_loader ();
        g_init.editor = editor_container_export ();

        finalize_test_case ();
        finalize_symlib ();
        g_init.to_run = true;
        invoke_test_case ( DBG_KERNEL_START );
        rest_init ();
        g_init.is_init = false;
}

__dlexport void kernel_halt ( void )
{
        g_init.to_run = false;
        invoke_test_case ( DBG_KERNEL_HALT );
}

static void rest_init ( void )
{
}

__dlexport void kernel_loop ( void )
{
        while ( g_init.to_run ) {
                static bool first_time = true;
                if ( first_time ) {
                        invoke_test_case ( DBG_KERNEL_LOOP_ONCE );
                        first_time = false;
                } else {
                        invoke_test_case ( DBG_KERNEL_LOOP );
                }
        }
}

__dlexport bool kernel_is_running ( void )
{
        return g_init.to_run;
}

__dlexport void kernel_init_param ( int argc, char **argv )
{
        g_init.argc = argc;
        g_init.argv = alloc_var ( sizeof ( char * ), argc );
        g_init.argv = expand_var ( g_init.argv, argc );
        int i;
        for ( i = 0; i < argc; i ++ ) {
                g_init.argv[i] = argv[i];
        }
}

__dlexport void kernel_add_param ( char *option, char *param )
{
        g_init.argv = add_var ( g_init.argv, 2 );
        int n = g_init.argc;
        g_init.argv[n + 0] =
                alloc_fix ( sizeof ( char ), strlen ( option ) + 1 );
        strcpy ( g_init.argv[n + 0], option );
        g_init.argv[n + 1] =
                alloc_fix ( sizeof ( char ), strlen ( param ) + 1 );
        strcpy ( g_init.argv[n + 1], param );
        g_init.argc += 2;
}

__dlexport void kernel_reg_environment ( void *env )
{
}

__dlexport void kernel_reg_signal ( char *name, f_Generic *sig_func )
{
}

__dlexport void kernel_ax_signal ( char *name )
{
}
