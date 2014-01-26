#ifndef X3DPARABOLOID_H_INCLUDED
#define X3DPARABOLOID_H_INCLUDED

#include "shape_common.h"

/*
 * Structures
 */

struct paraboloid_shape {
	struct shape_common prop;
	struct paraboloid3d pa;
	float phi1;
	struct shape_preserve pres;
};


#endif // X3DPARABOLOID_H_INCLUDED
