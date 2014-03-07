#ifndef X3DSHAPE_H_INCLUDED
#define X3DSHAPE_H_INCLUDED

#include <math/math.h>


/*
 * Structures
 */
/* Shared data that stores in each shape implementation */
struct shape_common {
        struct matrix4x4 t_world;
        struct matrix4x4 t_object;
        struct box3d bound;
};

struct shape_preserve {
        int pres[4];
};


/*
 * Definitions
 */

#define ObjectSharedAttrAddr(_shape, _attribute)		((char *) (_shape) + offsetof ( SHAPE_SHARED_DATA, _attribute ))
#define ObjectSharedAttr(_shape, _attribute)			((_shape)->prop._attribute)
#define get_obj_preserve( _pres, _TYPE, i )			((_TYPE *)_pres + i)

struct object_operations {
        void (*get_object_bound) ( void *obj, struct box3d *bound );
        void (*get_object_world_bound) ( void *obj, struct box3d *bound );
};

#include "trimesh_shape.h"



#endif // X3DSHAPE_H_INCLUDED
