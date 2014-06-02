#ifndef RENDERABLE_H_INCLUDED
#define RENDERABLE_H_INCLUDED

#include <x3d/common.h>
#include <algorithm.h>
#include <math.h>


enum RAG_CULL_IDR {
        RAG_CULL_NULL           = (0),
        RAG_CULL_CUBE           = (1 << 1),
        RAG_CULL_FRUSTUM        = (1 << 2),
        RAG_CULL_PYRAMID        = (1 << 3),
        RAG_CULL_OCCLUSION      = (1 << 4),
        RAG_CULL_LOD            = (1 << 5)
};

#define MAX_RENDERABLE_TYPE             6

enum RENDERABLE_IDR {
        RENDERABLE_GEOMETRY,
        RENDERABLE_SKINNED,
        RENDERABLE_PARTICLE,
        RENDERABLE_VOLUME,
        RENDERABLE_PROCEDURAL,
        RENDERABLE_TEXTURAL
};

enum RAG_IDR {
        RAG_LINEAR,
        RAG_STATIC_BVH,
        RAG_DYNAMIC_GRID
};

/*
 * structures
 */
struct material;
struct rda_instance;
typedef void rda_cullshape_t;

struct rda_request {
        uuid_t id;
        enum RENDERABLE_IDR type;
        enum RAG_CULL_IDR cull_method;
        rda_cullshape_t *cull_shape;
};

struct renderable {
        uuid_t id;
        char *name;
        enum RENDERABLE_IDR type;
        float importance;
        bool is_movable;
        bool is_dirty;
        struct material *mtl;
        struct rda_instance *insts;
        int ninsts;
};

struct rda_geometry {
        struct renderable _rda;
        int num_vertex;
        int num_tri;
        struct triangle_shape *tri;
        int *index;
        struct point3d *vertex;
        struct vector3d *normal;
        struct vector3d *tangent;
        struct point2d *uv;
};

struct rda_skinned {
        struct renderable _rda;
};

struct rda_particle {
        struct renderable _rda;
};

struct rda_textural {
        struct renderable _rda;
};

struct rda_instance {
        int i;
        struct renderable *rda;
        struct box3d bound;
        struct matrix4x4 transform;
};

struct rda_aggregate {
};

struct rda_context {
        struct rda_aggregate *agg;
        struct alg_hash_llist dict[MAX_RENDERABLE_TYPE];
        struct rda_instance **rda[MAX_RENDERABLE_TYPE];
};

/*
 * functions' declaration
 */
struct rda_context *create_renderable_context ( void );
void free_renderable_context ( struct rda_context *ctx );

/* renderable */
struct renderable *create_renderable (
        char *name, enum RENDERABLE_IDR type, float importance,
        bool is_movable, struct material *mtl );
void free_renderable ( struct renderable *rda );
void rda_set_name ( char *name, struct renderable *rda );
void rda_set_importance ( float importance, struct renderable *rda );
void rda_set_material ( struct material *mtl, struct renderable *rda );
char *rda_get_name ( struct renderable *rda );
struct rda_instance *rda_instance_create ( struct matrix4x4 *transform,
                                           struct renderable *rda );
void rda_instance_free ( struct rda_instance *inst );

/* geometry renderable */
void rda_geometry_init_from_data (
        struct point3d *vertex, int num_vert, int *index, int num_tri,
        struct vector3d *normal, struct vector3d *tangent, struct point2d *uv,
        struct matrix4x4 *transform, int ntransform, struct rda_geometry *geo );
void rda_geometry_refine ( float iteration, struct rda_geometry *geo );
void rda_geometry_update_vertex ( struct point3d *vertex, int count,
                                  struct rda_geometry *geo );
void rda_geometry_update_index ( int *index, int count, struct rda_geometry *geo );
void rda_geometry_update_transform ( int i, struct matrix4x4 *transform,
                                     struct rda_geometry *geo );
void rda_geometry_fix_nt ( struct rda_geometry *geo );

struct point3d *rda_geometry_get_vertex ( int *nvertex, struct rda_geometry *geo );
struct vector3d *rda_geometry_get_normal ( int *nnormal, struct rda_geometry *geo );
struct vector3d *rda_geometry_get_tangent ( int *ntangent, struct rda_geometry *geo );
struct vector2d *rda_geometry_get_uv ( int *nuv, struct rda_geometry *geo );
int *rda_geometry_get_index ( int *nindex, struct rda_geometry *geo );

/* aggregate */
void ragg_add_renderable (
        struct rda_instance *inst, enum RAG_IDR type, struct rda_aggregate *agg );
void ragg_update ( struct rda_aggregate *agg );

/* RI Interface */
void rda_context_post_request (
        enum RAG_CULL_IDR cull_type, rda_cullshape_t *shape,
        enum RENDERABLE_IDR rda_type, struct rda_request *request,
        struct rda_context *ctx );
struct rda_instance *rda_context_get_i ( int i, enum RENDERABLE_IDR type,
                                         struct rda_context *ctx );


#endif // RENDERABLE_H_INCLUDED
