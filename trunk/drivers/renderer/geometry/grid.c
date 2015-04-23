/* grid.c: Manage grid spatial primitive partition */
#include <container/arraylist.h>
#include <math/math.h>
#include <renderer/geometry.h>
#include <system/allocator.h>
#include "trimesh_shape.h"
#include "grid.h"

#define MIN_VOXELS		1
#define MAX_VOXELS		64


/* a subset of the primitives within a grid voxel */
#pragma pack(4)
struct voxel {
        int *prim_ref;
        int *nprim;
};
#pragma pack()

void to_grid_coord ( struct point3d *p, struct box3d *scale, struct point3d *inv_vw,
                     struct ipoint3d *p_out );


struct grid_spatial *build_grid_spatial ( struct alg_list *prim_list, int extra_params )
{
        struct grid_spatial *grid = alloc_fix ( sizeof ( *grid ), 1 );
        grid->prim_list = prim_list;

        /* Find out the scene scale to determine grid resolution */
        init_box3d ( &grid->scale );
        int i;
        for ( i = 0; i < alg_list_n ( prim_list ); i ++ ) {
                struct primitive *p =
                        &((struct primitive*) alg_array ( list, prim_list ))[i];
                struct box3d b;
                primitive_get_bound ( p, &b );
                union_box3d_u ( &grid->scale, &b );
        }

        /* Choose the axis with the longest span */
        struct vector3d span;
        sub_point3d ( &grid->scale.max, &grid->scale.min, &span );
        float maxs;
//	int axis;
        if ( span.x > span.y && span.x > span.z ) {
                maxs = span.x;
//		axis = VECTOR_X_AXIS;
        } else if ( span.y > span.z ) {
                maxs = span.y;
//		axis = VECTOR_Y_AXIS;
        } else {
                maxs = span.z;
//		axis = VECTOR_Z_AXIS;
        }
        copy_vector3d ( &span, &grid->span );

        /* Estimate grid resolution, nvoxel = 3*N^(1/3) */
        float n = 3.0f*powf ( alg_list_n ( prim_list ), 1.0f/3.0f );
        float dnds = n/maxs;

        for ( i = 0; i < 3; i ++ ) {
                grid->n_axis[i] = (int) (span.x*dnds + 0.5f);
                grid->n_axis[i] = clamp ( grid->n_axis[i], MIN_VOXELS, MAX_VOXELS );
        }

        /* Allocate memory for the grid structure */
        grid->nvoxel = grid->n_axis[0]*grid->n_axis[1]*grid->n_axis[2];
        grid->voxel = alloc_fix ( sizeof ( *grid->voxel ), grid->nvoxel );
        memset ( grid->voxel, 0, grid->nvoxel*sizeof ( *grid->voxel ) );

        /* Compute width of the voxel at each axis */
        for ( i = 0; i < 3; i ++ ) {
                float w = span.v[i]/grid->n_axis[i];
                float inv_w;
                if ( w == 0.0f ) {
                        inv_w = 0.0f;
                } else {
                        inv_w = 1.0f/w;
                }
                grid->vw.v[i] = w;
                grid->inv_vw.v[i] = inv_w;
        }

        /* Add primitives to grid voxels */
        for ( i = 0; i < alg_list_n ( prim_list ); i ++ ) {
                struct primitive *p =
                        &((struct primitive*) alg_array ( list, prim_list ))[i];

                /*  */
                struct box3d b;
                primitive_get_bound ( p, &b );
                struct ipoint3d p0;
                struct ipoint3d p1;
                to_grid_coord ( &b.min, &grid->scale, &grid->inv_vw, &p0 );
                to_grid_coord ( &b.max, &grid->scale, &grid->inv_vw, &p1 );

                /* Traverses all the voxels in the grid spatial
                 * to find out the voxels the current object overlapped,
                 * and add this object to those overlapped voxels */
                int j, k, l;
                for ( j = p0.z; j < p1.z; j ++ ) {
                        for ( k = p0.y; k < p1.y; k ++ ) {
                                for ( l = p0.x; l < p1.x; l ++ ) {
                                        int igrid = j*(grid->n_axis[1]*grid->n_axis[2]) +
                                                    k*(grid->n_axis[0]) + l;

                                        struct voxel *voxel = &grid->voxel[igrid];
                                        if ( voxel->prim_ref == nullptr ) {
                                                voxel->prim_ref = alloc_var ( sizeof ( int ), 1 );
                                        }
                                        voxel->prim_ref = push_var ( &i, &voxel->prim_ref );
                                        voxel->nprim ++;
                                }
                        }
                }
        }
        return grid;
}

/* Convert a 3d point to grid coordinate */
void to_grid_coord ( struct point3d *p, struct box3d *scale, struct point3d *inv_vw,
                     struct ipoint3d *p_out )
{
        int i;
        for ( i = 0; i < 3; i ++ ) {
                p_out->p[i] = (int) ((p->p[i] - scale->min.p[i])*inv_vw->p[i]);
        }
}

void free_grid_spatial ( struct grid_spatial *grid )
{
        int i;
        for ( i = 0; i < grid->nvoxel; i ++ ) {
                if ( grid->voxel[i].nprim > 0 ) {
                        free_var ( grid->voxel[i].prim_ref );
                }
        }
        free_fix ( grid );
}
