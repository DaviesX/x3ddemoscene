#ifndef X3DGRIDSPATIAL_H_INCLUDED
#define X3DGRIDSPATIAL_H_INCLUDED


#include <math/math.h>

struct alg_list;
struct voxel;

/*
 * Structures
 */

/* Defines a grid spatial accelerating structure */
struct grid_spatial {
        struct alg_list *prim_list;
        struct box3d scale;
        struct voxel *voxel;
        struct vector3d span;
        int nvoxel;
        int n_axis[3];
        struct vector3d vw;
        struct vector3d inv_vw;
};


/*
 * Functions' declarations
 */
struct grid_spatial *build_grid_spatial ( struct alg_list *prim_list, int extra_params );
void free_grid_spatial ( struct grid_spatial *grid );


#endif // X3DGRIDSPATIAL_H_INCLUDED
