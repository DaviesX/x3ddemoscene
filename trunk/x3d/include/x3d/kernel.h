#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED


#include <x3d/common.h>


/*
 * functions' declaration
 */
void kernel_init_param ( int argc, char **argv );
void kernel_add_param ( char *option, char *param );
void kernel_start ( void );
void kernel_halt ( void );
void kernel_loop_begin ( void );
void kernel_loop_end ( void );
bool kernel_loop_is_running ( void );
void kernel_panic ( void );


#endif // KERNEL_H_INCLUDED
