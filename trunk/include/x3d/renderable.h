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
struct shader;
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
        struct box3d bound;
        enum RENDERABLE_IDR type;
        float importance;
        bool is_movable;
        bool is_dirty;
        struct material *mtl;
        struct shader *sha;
};

struct rda_geometry {
        const struct renderable _rda;
        int num_vertex;
        int num_tri;
        struct triangle_shape *tri;
        int *index;
        struct point3d *vertex;
        struct vector3d *normal;
        struct vector3d *tangent;
        struct point2d *uv;
        struct matrix4x4 *transform;
        int ntransform;
};

struct rda_skinned {
        const struct renderable _rda;
};

struct rda_particle {
        const struct renderable _rda;
};

struct rda_textural {
        const struct renderable _rda;
};

struct rda_aggregate {
};

struct rda_context {
        struct rda_aggregate agg;
        struct alg_hash_llist dict[MAX_RENDERABLE_TYPE];
        untyped *rda[MAX_RENDERABLE_TYPE];
};

/*
 * functions' declaration
 */
struct rda_context *create_renderable_context ( void );
void free_renderable_context ( struct rda_context *ctx );

/* renderable */
struct renderable *create_renderable (
        char *name, enum RENDERABLE_IDR type, float importance, bool is_movable,
        struct material *mtl, struct shader *sha );
void free_renderable ( struct renderable *rda );
void rda_set_name ( char *name, struct renderable *rda );
void rda_set_importance ( float importance, struct renderable *rda );
void rda_set_material ( struct material *mtl, struct renderable *rda );
void rda_set_shader ( struct shader *sha, struct renderable *rda );

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
struct matrix4x4 *rda_geometry_get_transform ( int *ntransform, struct rda_geometry *geo );

/* aggregate */
void ragg_add_geometry (
        struct rda_geometry *geo, enum RAG_IDR type, struct rda_aggregate *agg );
void ragg_update ( struct rda_aggregate *agg );

/* RI Interface */
void rda_context_post_request (
        enum RAG_CULL_IDR cull_type, rda_cullshape_t *shape,
        enum RENDERABLE_IDR rda_type, struct rda_request *request,
        struct rda_context *ctx );
struct renderable *rda_context_get_i ( int i, enum RENDERABLE_IDR type,
                                       struct rda_context *ctx );


#endif // RENDERABLE_H_INCLUDED
