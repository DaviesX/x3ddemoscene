#include <stdio.h>
#include <x3d/kernel.h>


int main ( int argc, char *argv[] )
{
	init_startup_param ( argc, argv );
	add_startup_param ( "--edit-mode", "" );
	start_kernel ();
	return 0;
}
