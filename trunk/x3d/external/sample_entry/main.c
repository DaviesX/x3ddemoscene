#include <stdio.h>
#include <x3d/kernel.h>


int main ( int argc, char *argv[] )
{
	init_startup_param ( argc, argv );
	add_startup_param ( "--edit-mode", "" );
	add_startup_param ( "--test", "vert_post_process" );
	start_kernel ();
	return 0;
}
