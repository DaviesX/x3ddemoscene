#ifndef X3DHYPERBOLOID_H_INCLUDED
#define X3DHYPERBOLOID_H_INCLUDED

#include "shape_common.h"

/*
 * Structures
 */

struct hyperboloid_shape {
        struct shape_common prop;
        struct hyperboloid3d hy;
        float phi1;
        struct shape_preserve pres;
};


#endif // X3DHYPERBOLOID_H_INCLUDED
