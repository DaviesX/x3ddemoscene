#ifndef RENDERABLE_H_INCLUDED
#define RENDERABLE_H_INCLUDED

#include <x3d/common.h>
#include <algorithm.h>
#include <math.h>

/*
 * structure
 */
struct mtl_library {
};

/*
 * functions' declaration
 */


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
struct ragg;
struct renderable;
typedef void rda_cullshape_t;

struct rda_request {
        uuid_t                  id;
        enum RENDERABLE_IDR     type;
        enum RAG_CULL_IDR       cull_method;
        rda_cullshape_t*        cull_shape;

        struct rda_instance**   result;
        int                     n_result;
};

struct renderable {
        struct {
                void                    (*free) ( struct renderable* self );
                struct rda_instance*    (*instance_create) ( struct renderable *self, struct matrix4x4 *transform );
                void                    (*instance_free) ( struct rda_instance* inst );
        };
        uuid_t                  id;
        char*                   name;
        enum RENDERABLE_IDR     type;
        float                   importance;
        bool                    is_movable;
        bool                    is_dirty;
        int                     mater_ref;
        struct rda_instance**   insts;
        int ninsts;
};

struct rda_geometry {
        struct renderable       _parent;
        int                     num_vertex;
        int                     num_tri;
        struct triangle_shape*  tri;
        int*                    index;
        struct point3d*         vertex;
        struct vector3d*        normal;
        struct vector3d*        tangent;
        struct point2d*         uv;
};

struct rda_skinned {
        struct renderable       _parent;
};

struct rda_particle {
        struct renderable       _parent;
};

struct rda_textural {
        struct renderable       _parent;
};

struct rda_instance {
        int                     i;
        struct renderable*      rda_link;
        struct box3d            bound;
        struct matrix4x4        transform;
};

struct rda_context {
        struct ragg*   agg;
        struct alg_hash_llist   dict[MAX_RENDERABLE_TYPE];
        struct mtl_library      mtllib;
        struct rda_request      request[128];
        int                     n_request;
};

/*
 * functions' declaration
 */
/* renderable */
struct renderable*      rda_create ( char *name, enum RENDERABLE_IDR type,
                                     float importance, bool is_movable,
                                     int mater_ref );
void                    rda_free ( struct renderable *rda );
void                    rda_set_name ( struct renderable *rda, char *name );
void                    rda_set_importance ( struct renderable *rda, float importance );
void                    rda_set_material ( struct renderable *rda, int mater_ref );
char*                   rda_get_name ( struct renderable *rda );
struct rda_instance*    rda_instance_create ( struct renderable *rda, struct matrix4x4 *transform );
void                    rda_instance_free ( struct rda_instance *inst );
struct renderable*      rda_instance_source ( struct rda_instance* inst );

/* geometry renderable */
void rda_geometry_init_from_data ( struct rda_geometry *geo,
                                   struct point3d* vertex, int num_vert, int* index, int num_tri,
                                   struct vector3d* normal, struct vector3d* tangent, struct point2d* uv,
                                   struct matrix4x4* transform );
void rda_geometry_refine ( struct rda_geometry* geo, float iteration );
void rda_geometry_update_vertex ( struct rda_geometry* geo, struct point3d* vertex, int count );
void rda_geometry_update_index ( struct rda_geometry* geo, int* index, int count );
void rda_geometry_set_transform ( struct rda_geometry* geo, struct matrix4x4 *transform );
void rda_geometry_fix_nt ( struct rda_geometry* geo );

struct point3d*         rda_geometry_get_vertex ( struct rda_geometry* geo, int *nvertex );
struct vector3d*        rda_geometry_get_normal ( struct rda_geometry* geo, int* nnormal );
struct vector3d*        rda_geometry_get_tangent ( struct rda_geometry* geo, int* ntangent );
struct vector2d*        rda_geometry_get_uv ( struct rda_geometry* geo, int* nuv );
int*                    rda_geometry_get_index ( struct rda_geometry* geo, int* nindex );


/* renderable context */
struct rda_context*     rda_context_create ( enum RAG_IDR agg_type );
void                    rda_context_free ( struct rda_context* ctx );
void                    rda_context_add_instance ( struct rda_context* ctx,
                                                   struct rda_instance* insts,
                                                   enum RENDERABLE_IDR type );
void                    rda_context_update ( struct rda_context* ctx );
/* RI Interface */
uuid_t                  rda_context_post_request ( struct rda_context* ctx,
                                enum RAG_CULL_IDR cull_type, rda_cullshape_t* shape,
                                enum RENDERABLE_IDR rda_type );
int                     rda_context_get_n ( struct rda_context* ctx, uuid_t request_id );
struct rda_instance*    rda_context_get_i ( struct rda_context* ctx, int i, uuid_t request_id );


#endif // RENDERABLE_H_INCLUDED
