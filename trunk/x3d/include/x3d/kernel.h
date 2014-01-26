#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED


/*
 * functions' declaration
 */
void init_startup_param ( int argc, char **argv );
void add_startup_param ( char *option, char *param );
void start_kernel ( void );
void shutdown_kernel ( void );
void kernel_panic ();


#endif // KERNEL_H_INCLUDED
