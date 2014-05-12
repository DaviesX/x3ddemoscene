/* renderable.c: manages all kinds of renderable and
 * connect to local renderable manager interface */
#include <x3d/renderable.h>


/* @todo (davis#2#): <kernel/renderable> implement geometry renderable and simple aggregate */

struct renderable_ctx *create_renderable_ctx ( void )
{
        return nullptr;
}

void free_renderable_ctx ( struct renderable_ctx *ctx )
{
}

/* geometry renderable */
struct geometry_mgr;
struct geometry;

struct georend_ops {
        struct geometry_mgr *(*create_mgr) ( void );
        void (*reset_mgr) ( struct geometry_mgr *mgr );
        void (*get_render_desc) ( struct geometry_mgr *mgr,
                                  struct alg_list *desc );

        struct geometry *(*create_geometry) ( void );
        void (*update_geometry) ( void );
        void (*add_geometry) ( struct geometry *geo, struct render_desc *desc,
                               struct geometry_mgr *mgr );
        void (*get_geometry_bound) ( struct geometry *geo, struct box3d *bound );
};

/*static*/ struct georend_ops g_georend_ops = {
        .create_mgr = nullptr,
        .reset_mgr = nullptr,
        .get_render_desc = nullptr,
        .create_geometry = nullptr,
        .update_geometry = nullptr,
        .add_geometry = nullptr,
        .get_geometry_bound = nullptr
};


struct geometry_renderable *renderable_create_geometry ( void )
{
        return nullptr;
}

void renderable_update_geometry ( void )
{
}

void renderable_ctx_add_geometry ( struct geometry_renderable *geo,
                                   struct renderable_ctx *ctx )
{
}

void renderable_ctx_add_from_aggregate ( struct rdb_aggregate *agg,
                struct renderable_ctx *ctx )
{
}

void init_rdb_aggregate ( struct rdb_aggregate *agg )
{
}

void free_rdb_aggregate ( struct rdb_aggregate *agg )
{
}

void rdb_aggregate_add_geometry (
        struct geometry_renderable *geo, enum RDB_AGGREG type,
        struct rdb_aggregate *agg )
{
}

/* RI Interface */
void krender_desc_add (
        enum RENDER_DESC_CULL method, void *shape, uint32_t renderable_type,
        struct alg_list *desc )
{
}
struct renderable_mgr *krenderable_ctx_get_mgr (
        enum RENDERABLE type, struct renderable_ctx *ctx )
{
        return nullptr;
}
