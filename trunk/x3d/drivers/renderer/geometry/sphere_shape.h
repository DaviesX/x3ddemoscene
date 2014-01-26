#ifndef X3DSPHERE_H_INCLUDED
#define X3DSPHERE_H_INCLUDED

#include "shape_common.h"

/*
 * Structures
 */

struct sphere_shape {
	struct shape_common prop;
	struct sphere3d sp;
	float the0, the1;
	float phi1;
	struct shape_preserve pres;
};


#endif // X3DSPHERE_H_INCLUDED
