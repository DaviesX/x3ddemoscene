#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED


#include <misc.h>

/*
 * functions' declaration
 */
__dlexport int kernel_main ( int argc, char** argv );
__dlexport void kernel_add_param(const char *option, const char *param);
__dlexport bool kernel_start ( void );
__dlexport void kernel_halt ( void );
__dlexport void kernel_loop ( void );
__dlexport void kernel_shutdown ( void );
__dlexport bool kernel_is_running ( void );
__dlexport void kernel_panic ( void );

typedef int (*f_On_Init) ( int argc, char **argv, void *env );
typedef int (*f_On_Rest_Init) ( void *env );
typedef int (*f_On_Loop_Init) ( void *env );
typedef int (*f_On_Loop) ( void *env );
typedef int (*f_On_Loop_Free) ( void *env );
typedef int (*f_On_Free) ( void *env );

__dlexport void kernel_reg_environment ( void *env );
__dlexport void kernel_reg_signal ( char *name, f_Generic sig_func );
__dlexport void kernel_ax_signal ( char *name );


#endif // INIT_H_INCLUDED
