#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED


#include <x3d/common.h>


/*
 * functions' declaration
 */
__dlexport void kernel_init_param ( int argc, char **argv );
__dlexport void kernel_add_param ( char *option, char *param );
__dlexport void kernel_start ( void );
__dlexport void kernel_halt ( void );
__dlexport void kernel_loop ( void );
__dlexport bool kernel_is_running ( void );
__dlexport void kernel_panic ( void );

typedef (*f_On_Init) ( int argc, char **argv, void *env );
typedef (*f_On_Rest_Init) ( void *env );
typedef (*f_On_Loop_Init) ( void *env );
typedef (*f_On_Loop) ( void *env );
typedef (*f_On_Loop_Free) ( void *env );
typedef (*f_On_Free) ( void *env );

__dlexport void kernel_reg_environment ( void *env );
__dlexport void kernel_reg_signal ( char *name, f_Generic *sig_func );
__dlexport void kernel_ax_signal ( char *name );


#endif // INIT_H_INCLUDED
