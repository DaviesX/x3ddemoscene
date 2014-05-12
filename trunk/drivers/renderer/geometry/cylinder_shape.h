#ifndef X3DCYLINDER_H_INCLUDED
#define X3DCYLINDER_H_INCLUDED


/*
 * Include relative headers
 */
#include "shape_common.h"

/*
 * Structures
 */
struct cylinder_shape {
        struct shape_common common;
        struct cylinder3d *cy;
        float phi1;
        struct shape_preserve pres;
};


#endif // X3DCYLINDER_H_INCLUDED
