#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED


#include <x3d/common.h>
#include <x3d/resource_loader.h>
#include <math/math.h>


#define primitive	triangle_shape
#define stdshape	triangle_mesh_shape

enum PRIMPART_IDR {
        PRIMITIVE_PART_NULL,
        PRIMITIVE_PART_GRID,
        PRIMITIVE_PART_BVH,
        PRIMITIVE_PART_KD,
        PRIMITIVE_PART_OCT
};

struct frustum3d;
struct box3d;
struct matrix4x4;
struct alg_list;
struct primitive;
struct prim_part;

/*
 * Structures
 */
/* primitive partition */
struct prim_part {
        enum PRIMPART_IDR idr;
        struct alg_list *prim;
        void *parter;
};

struct primpart_iterator {
        uint32_t pos;
};

/* shape */
enum SHAPE_IDR {
        SHAPE_IDR_SPHERE,
        SHAPE_IDR_CONE,
        SHAPE_IDR_CYLINDER,
        SHAPE_IDR_DISK,
        SHAPE_IDR_PARABOLOID,
        SHAPE_IDR_HYPERBOLOID,
        SHAPE_IDR_NURB,
        SHAPE_IDR_TRIANGLEMESH
};

struct shape {
        enum SHAPE_IDR idr;
        struct prim_part parter;
        void *shape;
};

/* geometry */
struct geometry {
        res_handle_t shape;	/* stdshape */
        res_handle_t mat;	/* material */
        struct matrix4x4 trans;
};


/*
 * shape's declaration
 */
void init_shape ( void *shape_data, enum SHAPE_IDR idr, struct shape *shape );
void free_shape ( struct shape *shape );
struct stdshape *shape_to_stdshape ( struct shape *shape );
void shape_get_primitives ( struct shape *shape, struct alg_list *prim );
void shape_get_primitives_frustum ( struct shape *shape, struct frustum3d *f,
                                    struct alg_list *prim );
void shape_get_primitives_box ( struct shape *shape, struct box3d *b,
                                struct alg_list *prim );
void primitive_get_bound ( struct primitive *prim, struct box3d *bound );

/*
 * geometry's declaration
 */
void create_geometry ( res_handle_t shape, res_handle_t mater,
                       res_handle_t shader, struct geometry *geo );
void free_geometry ( struct geometry *geo );
void geometry_get_shape ( struct geometry *geo, struct shape *shape );
void geometry_set_transform ( struct matrix4x4 *trans, struct geometry *geo );

/*
 * primitive partition's declaration
 */
void build_primitive_partition ( struct alg_list *prim, enum PRIMPART_IDR idr, int extra_params,
                                 struct prim_part *pp );
void free_primitive_partition ( struct prim_part *pp );

bool prim_part_null_first ( struct prim_part *pp, struct primpart_iterator *iter,
                            struct primitive *prim );
bool prim_part_null_next ( bool positivity, struct primpart_iterator *iter,
                           struct primitive *prim );

/* #include "cone_shape.h"
#include "cylinder_shape.h"
#include "disk_shape.h"
#include "hyperboloid_shape.h"
#include "paraboloid_shape.h"
#include "sphere_shape.h" */
// #include <renderer/geometry/trimesh_shape.h>


#endif // GEOMETRY_H_INCLUDED
