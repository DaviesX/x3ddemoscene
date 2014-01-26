#ifndef X3DTRIANGLEMESH_H_INCLUDED
#define X3DTRIANGLEMESH_H_INCLUDED


/*
 * Include relative headers
 */
#include "shape_common.h"

enum TRIANGLE_MESH_ATTR_IDR {
	TMA_IDR_VERTEX,
	TMA_IDR_NORMAL,
	TMA_IDR_TANGENT,
	TMA_IDR_UV
};

/*
 * Structures
 */

struct triangle_mesh_shape {
	struct shape_common prop;
	int num_vertex;
	int num_tri;
	struct triangle_shape *tri;
	int *i;
	struct point3d *v;
	struct vector3d *n;
	struct vector3d *t;
	struct point2d *uv;
	void *alpha;
	struct shape_preserve preserve;
};

struct triangle_shape {
	struct triangle_mesh_shape *mesh;
	int *i;
};


/*
 * Functions' declaration
 */
void build_triangle_mesh ( struct point3d *vertex, int num_vert, int *index, int num_tri,
			   struct vector3d *normal, struct vector3d *tangent, struct point2d *uv,
			   struct point3d *scale, struct point3d *rotation, struct point3d *world_pos,
			   int is_trans, struct triangle_mesh_shape *mesh );
void free_triangle_mesh ( struct triangle_mesh_shape *mesh );
void set_transform_triangle_mesh ( struct matrix4x4 *t_world, struct matrix4x4 *t_object,
				   struct triangle_mesh_shape *mesh );
void transform_to_world_triangle_mesh ( struct triangle_mesh_shape *mesh );
void get_triangle3d_from_shape ( struct triangle_shape *triangle, struct triangle3d *tri_out );
void get_triangle4d_from_shape ( struct triangle_shape *triangle, struct triangle4d *tri_out );
void transform_triangle ( struct triangle4d *tri, struct matrix4x4 *transform, struct triangle4d *tri_out );
void transform_triangle_u ( struct triangle4d *tri, struct matrix4x4 *transform );
void get_triangle_mesh_bound ( struct triangle_mesh_shape *mesh, struct box3d *bound );
void get_triangle_mesh_world_bound ( struct triangle_mesh_shape *mesh, struct box3d *bound );
void get_triangle_bound ( struct triangle_shape *t, struct box3d *bound );


#endif // X3DTRIANGLEMESH_H_INCLUDED
