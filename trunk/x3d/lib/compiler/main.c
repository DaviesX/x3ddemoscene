#include <x3d/common.h>
#include <compiler/compiler.h>


int main ( void )
{
	const char *code = "8+ 3*5 + 6*7;";
	printf ( "code: %s\n", code );
	struct compiler c;
	init_compiler ( &c );
	comile_c_to_native ( code, &c );
	free_compiler ( &c );
	return 0;
}
