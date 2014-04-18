#include <stdio.h>
#include <x3d/init.h>
#include <x3d/editor.h>


int main ( int argc, char *argv[] )
{
        kernel_init_param ( argc, argv );
//        kernel_add_param ( "--test", "vert_post_process" );
//        kernel_add_param ( "--test", "simple_rt_pipeline" );
//        kernel_add_param ( "--test", "llist_container_c" );
        kernel_start ();
        editor_enter_loop ( false );
        while ( kernel_loop_is_running () ) {
                kernel_loop_begin ();
                kernel_loop_end ();
        }
        return 0;
}
