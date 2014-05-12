#ifndef CONE_SHAPE_H_INCLUDED
#define CONE_SHAPE_H_INCLUDED


#include "shape_common.h"

/*
 * Structures
 */

struct cone_shape {
        struct shape_common common;
        struct cone3d c;
        float phi1;
        struct shape_preserve *pres;
};


#endif // CONE_SHAPE_H_INCLUDED
