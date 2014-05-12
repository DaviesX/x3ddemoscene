#ifndef RENDERABLE_H_INCLUDED
#define RENDERABLE_H_INCLUDED

#include <x3d/common.h>
#include <algorithm.h>

struct renderable_mgr;
struct object_entity;

enum RENDER_DESC_CULL {
        RENDER_DESC_CULL_NULL           = 0X0,
        RENDER_DESC_CULL_CUBE           = 0X1,
        RENDER_DESC_CULL_FRUSTUM        = 0X2,
        RENDER_DESC_CULL_OCCLUSION      = 0X4,
        RENDER_DESC_CULL_LOD            = 0X8
};

enum RENDERABLE {
        RENDERABLE_GEOMETRY     = 0X1,
        RENDERABLE_VOLUME       = 0X2,
        RENDERABLE_PROCEDURAL   = 0X4,
        RENDERABLE_TEXTURE      = 0X8
};

enum RDB_AGGREG {
        RDB_AGGREG_SIMPLE,
        RDB_AGGREG_STATIC_BVH,
        RDB_AGGREG_DYNAMIC_GRID
};

/*
 * structures
 */
struct render_desc {
        uuid_t id;
        uint32_t cull_method;
        void *shape;
        uint32_t rend_type;
};

struct renderable {
//        enum RENDERABLE type;
        int prio;
        struct box3d bound;
        struct object_entity *entity;
        void *rendbl;
};

struct geometry_renderable {
        const struct renderable rdb;
        void *rendbl;
};

struct volume_renderable {
        const struct renderable rdb;
        void *rendbl;
};

struct procedural_renderable {
        const struct renderable rdb;
        void *rendbl;
};

struct texture_renderable {
        const struct renderable rdb;
        void *rendbl;
};

struct rdb_aggregate {
};

struct renderable_ctx {
        struct alg_list desc;
        struct renderable_mgr *mgr[4];
};

/*
 * functions' declaration
 */
struct renderable_ctx *create_renderable_ctx ( void );
void free_renderable_ctx ( struct renderable_ctx *ctx );

/* geometry renderable */
struct geometry_renderable *renderable_create_geometry ( void );
void renderable_update_geometry ( void );
void renderable_ctx_add_geometry ( struct geometry_renderable *geo,
                                   struct renderable_ctx *ctx );
void renderable_ctx_add_from_aggregate ( struct rdb_aggregate *agg,
                struct renderable_ctx *ctx );
void init_rdb_aggregate ( struct rdb_aggregate *agg );
void free_rdb_aggregate ( struct rdb_aggregate *agg );
void rdb_aggregate_add_geometry (
        struct geometry_renderable *geo, enum RDB_AGGREG type,
        struct rdb_aggregate *agg );

/* RI Interface */
void krender_desc_add (
        enum RENDER_DESC_CULL method, void *shape, uint32_t renderable_type,
        struct alg_list *desc );
struct renderable_mgr *krenderable_ctx_get_mgr (
        enum RENDERABLE type, struct renderable_ctx *ctx );


#endif // RENDERABLE_H_INCLUDED
