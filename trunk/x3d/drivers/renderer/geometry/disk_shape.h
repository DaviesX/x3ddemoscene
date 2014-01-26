#ifndef X3DDISK_H_INCLUDED
#define X3DDISK_H_INCLUDED


/*
 * Include relative headers
 */
#include "shape_common.h"


struct disk_shape {
	struct shape_common prop;
	struct disk3d *d;
	float phi1;
	struct shape_preserve pres;
};


#endif // X3DDISK_H_INCLUDED
