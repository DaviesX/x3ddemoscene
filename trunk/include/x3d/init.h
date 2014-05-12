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
__dlexport void kernel_loop_begin ( void );
__dlexport void kernel_loop_end ( void );
__dlexport bool kernel_loop_is_running ( void );
__dlexport void kernel_panic ( void );


#endif // INIT_H_INCLUDED
