#include <stdio.h>
#include <x3d/kernel.h>


int main ( int argc, char *argv[] )
{
        kernel_init_param ( argc, argv );
        kernel_add_param ( "--test", "vert_post_process" );
        kernel_add_param ( "--test", "simple_rt_pipeline" );
        kernel_start ();
        while ( kernel_loop_is_running () ) {
                kernel_loop_begin ();
                kernel_loop_end ();
        }
        return 0;
}
