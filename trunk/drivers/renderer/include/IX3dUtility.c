#include "X3dRenderLibrary.h"



x3d_named_params_t *create_named_params_x3d ( void )
{
	x3d_named_params_t *np = alloc_fix ( sizeof ( *np ), 1 );
	init_named_params ( np );
	return np;
}

void free_named_params_x3d ( x3d_named_params_t *np )
{
	free_named_params ( np );
	free_fix ( np );
}
