/* renderable.c: manages all kinds of renderable and connect to local renderable manager interface */
#include <math/math.h>
#include <logout.h>
#include <x3d/renderable.h>


/* @todo (davis#2#): <kernel/renderable> implement geometry renderable and simple aggregate */
static const int c_sizeof_renderable[] = {
        [RENDERABLE_GEOMETRY] = sizeof (struct rda_geometry)
};

/* geometry renderable */
static void rda_geometry_init ( struct rda_geometry* geo )
{
        geo->index      = nullptr;
        geo->num_tri    = 0;
        geo->normal     = nullptr;
        geo->vertex     = nullptr;
        geo->uv         = nullptr;
        geo->tangent    = nullptr;
        geo->num_tri    = 0;
}

static void rda_geometry_free ( struct rda_geometry *geo )
{
        free_fix ( geo->index );
        geo->num_tri    = 0;
        free_fix ( geo->normal );
        free_fix ( geo->vertex );
        free_fix ( geo->uv );
        free_fix ( geo->tangent );
        geo->num_tri    = 0;
}

static void rda_geometry_get_bound ( struct rda_geometry* geo, struct matrix4x4* transform,
                                     struct box3d* bound )
{
        init_box3d ( bound );
        int i;
        for ( i = 0; i < geo->num_vertex; i ++ ) {
                struct point3d p;
                mul_point3d_matrix4x4 ( &geo->vertex[i], transform, &p );
                union_box3d_pu ( bound, &p );
        }
}

static struct rda_geometry* rda_geometry_make_copy ( struct rda_geometry* geo )
{
        log_mild_err_dbg ( "no makecopy yet" );
        return nullptr;
}

void rda_geometry_init_from_data ( struct rda_geometry *geo,
                                   struct point3d* vertex, int num_vert, int* index, int num_tri,
                                   struct vector3d* normal, struct vector3d* tangent, struct point2d* uv,
                                   struct matrix4x4* pre_transform )
{
        /* index */
        geo->index      = alloc_fix ( sizeof(int), num_tri*3 );
        geo->num_tri    = num_tri;
        memcpy ( geo->index, index, sizeof(int)*num_tri*3 );
        /* vertex */
        geo->vertex     = alloc_fix ( sizeof(struct point3d), num_vert );
        geo->num_vertex = num_vert;
        memcpy ( geo->vertex, vertex, sizeof(struct point3d)*num_vert );
        /* normal */
        geo->normal     = alloc_fix ( sizeof(struct vector3d), num_vert );
        memcpy ( geo->normal, normal, sizeof(struct vector3d)*num_vert );
        /* tangent */
        geo->tangent     = alloc_fix ( sizeof(struct vector3d), num_vert );
        memcpy ( geo->tangent, tangent, sizeof(struct vector3d)*num_vert );
        /* uv */
        geo->uv         = alloc_fix ( sizeof(struct vector2d), num_vert );
        memcpy ( geo->uv, uv, sizeof(struct vector2d)*num_vert );
/* @fixme (davis#9#): <geometry renderable> add pre-transform */
        log_mild_err_dbg ( "pre-transform not supported yet" );
}

void rda_geometry_refine ( struct rda_geometry* geo, float iteration )
{
}

void rda_geometry_update_vertex ( struct rda_geometry* geo, struct point3d* vertex, int count )
{
}

void rda_geometry_update_index ( struct rda_geometry* geo, int* index, int count )
{
}

void rda_geometry_set_transform ( struct rda_geometry* geo, struct matrix4x4 *transform )
{
}

void rda_geometry_fix_nt ( struct rda_geometry* geo )
{
}

struct point3d* rda_geometry_get_vertex ( struct rda_geometry* geo, int *nvertex )
{
        *nvertex = geo->num_vertex; return geo->vertex;
}

struct vector3d* rda_geometry_get_normal ( struct rda_geometry* geo, int* nnormal )
{
        *nnormal = geo->num_vertex; return geo->normal;
}

struct vector3d* rda_geometry_get_tangent ( struct rda_geometry* geo, int* ntangent )
{
        *ntangent = geo->num_vertex; return geo->tangent;
}

struct vector2d* rda_geometry_get_uv ( struct rda_geometry* geo, int* nuv )
{
        *nuv = geo->num_vertex; return geo->uv;
}

int* rda_geometry_get_index ( struct rda_geometry* geo, int* nindex )
{
        *nindex = geo->num_tri*3; return geo->index;
}

/* renderable */
static const struct rda_operation {
        void (*init) ( struct renderable* self );
        void (*free) ( struct renderable* self );
        struct renderable* (*make_copy) ( struct renderable* self );
        void (*get_bound) ( struct renderable* self, struct matrix4x4* t, struct box3d* b );
} cRdaOps[] = {
        [RENDERABLE_GEOMETRY].init = cast(cRdaOps->init)                rda_geometry_init,
        [RENDERABLE_GEOMETRY].free = cast(cRdaOps->free)                rda_geometry_free,
        [RENDERABLE_GEOMETRY].make_copy =  cast(cRdaOps->make_copy)     rda_geometry_make_copy,
        [RENDERABLE_GEOMETRY].get_bound = cast(cRdaOps->get_bound)      rda_geometry_get_bound
};

static void h_renderable_init ( struct renderable* rda,
                                char *name, enum RENDERABLE_IDR type, float importance,
                                bool is_movable, int mater_ref )
{
        rda->id         = alg_gen_uuid ( );
        rda->importance = importance;
        rda->name       = alg_alloc_string ( name );
        rda->is_dirty   = true;
        rda->is_movable = is_movable;
        rda->mater_ref  = mater_ref;
        rda->insts      = alloc_var ( sizeof(struct rda_instance*), 1 );
        rda->type       = type;
}

static void h_renderable_free ( struct renderable* rda )
{
        free_var ( rda->insts );
        free_fix ( rda->name );
        zero_obj ( rda );
}

struct renderable* rda_create ( char *name, enum RENDERABLE_IDR type,
                                float importance, bool is_movable,
                                int mater_ref )
{
        struct renderable* rda = nullptr;
        switch ( type ) {
        case RENDERABLE_GEOMETRY:;
                struct rda_geometry* rgeo = alloc_obj ( rgeo );
                h_renderable_init ( (struct renderable*) rgeo,
                                    name, type, importance, is_movable, mater_ref );
                rda = (struct renderable*) rgeo;
                break;
        case RENDERABLE_PARTICLE:
                break;
        case RENDERABLE_SKINNED:
                break;
        case RENDERABLE_TEXTURAL:
                break;
        case RENDERABLE_VOLUME:
                break;
        case RENDERABLE_PROCEDURAL:
                break;
        }

        cRdaOps[type].init ( rda );
        return rda;
}

void rda_free ( struct renderable *rda )
{
        cRdaOps[rda->type].free ( rda );
        h_renderable_free ( rda );
        zero_obj ( rda );
        free_fix ( rda );
}

void rda_set_name ( struct renderable *rda, char *name )
{
        free_fix ( name );
        rda->name = alg_alloc_string ( name );
}

void rda_set_importance ( struct renderable *rda, float importance )
{
        rda->importance = importance;
}

void rda_set_material ( struct renderable *rda, int mater_ref )
{
        rda->mater_ref = mater_ref;
}

char* rda_get_name ( struct renderable *rda )
{
        return rda->name;
}

struct rda_instance* rda_instance_create ( struct renderable *rda, struct matrix4x4 *transform )
{
        struct rda_instance* insts = alloc_obj ( insts );
        insts->transform        = *transform;
        insts->i                = rda->ninsts;
        insts->rda_link         = rda;
        cRdaOps[rda->type].get_bound ( rda, transform, &insts->bound );

        rda->insts = add_var ( rda->insts, 1 );
        rda->insts[rda->ninsts ++] = insts;
        return insts;
}

void rda_instance_free ( struct rda_instance *inst )
{
/* @fixme (davis#9#): <renderable instance>: free resource from renderable entity also */
        free_fix ( inst );
}

struct renderable* rda_instance_source ( struct rda_instance* inst )
{
        return inst->rda_link;
}

/* renderable context & aggregate */
/* aggregate */
struct ragg;

struct ragg {
        void (*init) ( struct ragg* self );
        void (*update) ( struct ragg* self );
        void (*free) ( struct ragg* self );
        void (*add) ( struct ragg* self, enum RENDERABLE_IDR type, struct rda_instance* inst );
        void (*remove) ( struct ragg* self, enum RENDERABLE_IDR type, struct rda_instance* inst );
        struct rda_instance** (*frustum_find) ( struct ragg* self, enum RENDERABLE_IDR type, struct frustum3d* f );
};

struct ragg_linear {
        struct ragg             _parent;
        struct rda_instance**   insts[MAX_RENDERABLE_TYPE];
};

struct ragg_bvh {
        struct ragg             _parent;
};

struct ragg_grid {
        struct ragg             _parent;
};

/* linear ragg */
static void ragg_linear_init ( struct ragg_linear* agg )
{
        int i;
        for ( i = 0; i < sizeof(agg->insts)/sizeof(char*); i ++ ) {
                agg->insts[i] = alloc_var ( sizeof(struct rda_instance*), 1 );
        }
}

static void ragg_linear_update ( struct ragg_linear* agg )
{
        /* do nothing */
}

static void ragg_linear_free ( struct ragg_linear* agg )
{
        int i;
        for ( i = 0; i < sizeof(agg->insts)/sizeof(char*); i ++ ) {
                free_var ( agg->insts[i] );
        }
}

static void ragg_linear_add ( struct ragg_linear* agg, enum RENDERABLE_IDR type, struct rda_instance* inst )
{
        int n = get_var_len(agg->insts[type]);
        agg->insts[type] = add_var ( agg->insts[type], 1 );
        agg->insts[type][n] = inst;
}

static void ragg_linear_remove ( struct ragg_linear* agg, enum RENDERABLE_IDR type, struct rda_instance* inst )
{
        /* no remove functionality */
        log_mild_err_dbg ( "linear renderable aggregate has no remove functionality" );
}

static struct rda_instance** ragg_linear_frustum_find ( struct ragg_linear* agg,
                                       enum RENDERABLE_IDR type,
                                       struct frustum3d* f )
{
/* @fixme (davis#1#): find intersection against the frustum */
        return agg->insts[type];
}

/* aggregate definition */
static struct ragg* ragg_create ( enum RAG_IDR type )
{
        struct ragg* agg = nullptr;
        /* allocate new object and link its operations */
        switch ( type ) {
        case RAG_LINEAR:
                agg = alloc_fix ( sizeof(struct ragg_linear), 1 );
                agg->init = cast(agg->init)                     ragg_linear_init;
                agg->free = cast(agg->free)                     ragg_linear_free;
                agg->update = cast(agg->update)                 ragg_linear_update;
                agg->remove = cast(agg->remove)                 ragg_linear_remove;
                agg->frustum_find = cast(agg->frustum_find)     ragg_linear_frustum_find;
                break;
        case RAG_STATIC_BVH:
                break;
        case RAG_DYNAMIC_GRID:
                break;
        }
        /* initialize the object */
        agg->init ( agg );
        return agg;
}

static void ragg_free ( struct ragg* agg )
{
        agg->free ( agg );
}

static void ragg_update ( struct ragg *agg )
{
        agg->update ( agg );
}

static void ragg_add ( struct ragg* agg, enum RENDERABLE_IDR type, struct rda_instance* inst )
{
        agg->add ( agg, type, inst );
}

static void ragg_remove ( struct ragg* agg, enum RENDERABLE_IDR type, struct rda_instance* inst )
{
        agg->remove ( agg, type, inst );
}

static struct rda_instance** ragg_frustum_find ( struct ragg* agg,
                                enum RENDERABLE_IDR type,
                                struct frustum3d* f )
{
        return agg->frustum_find ( agg, type, f );
}

/* RI Interface */
uuid_t rda_context_post_request ( struct rda_context* ctx,
                                  enum RAG_CULL_IDR cull_type, rda_cullshape_t* shape,
                                  enum RENDERABLE_IDR rda_type )
{
        struct rda_request req;
        req.cull_shape = shape;
        req.cull_method = cull_type;
        req.id = ctx->n_request ++;
        ctx->request[req.id] = req;
        return req.id;
}

int rda_context_get_n ( struct rda_context* ctx, uuid_t request_id )
{
        return ctx->request[request_id].n_result;
}

struct rda_instance* rda_context_get_i ( struct rda_context* ctx, int i, uuid_t request_id )
{
        return ctx->request[request_id].result[i];
}

static void rda_request_init ( struct rda_request* req )
{
        req->cull_shape = nullptr;
        req->result = alloc_var ( sizeof(struct rda_request*), 1 );
        req->n_result = 0;
}

static void rda_request_free ( struct rda_request* req )
{
        free_var ( req->result );
        zero_obj ( req );
}

/* renderable context */
struct rda_context* rda_context_create ( enum RAG_IDR agg_type )
{
        struct rda_context* ctx = alloc_obj ( ctx );
        ctx->agg = ragg_create ( agg_type );
        int i;
        for ( i = 0;
              i < sizeof(ctx->request)/sizeof(struct rda_request);
              i ++ ) {
                rda_request_init ( &ctx->request[i] );
        }
        ctx->n_request = 0;
        return ctx;
}

void rda_context_free ( struct rda_context* ctx )
{
        ragg_free ( ctx->agg );
        int i;
        for ( i = 0;
              i < sizeof(ctx->request)/sizeof(struct rda_request);
              i ++ ) {
                rda_request_free ( &ctx->request[i] );
        }
        ctx->n_request = 0;
        free_fix ( ctx );
}

void rda_context_add_instance ( struct rda_context* ctx,
                                struct rda_instance* inst,
                                enum RENDERABLE_IDR type )
{
        ragg_add ( ctx->agg, type, inst );
}

void rda_context_update ( struct rda_context* ctx )
{
        ragg_update ( ctx->agg );
        /* process all renderable requests */
        int i;
        for ( i = 0; i < ctx->n_request; i ++ ) {
                struct rda_request* c = &ctx->request[i];
                switch ( c->cull_method ) {
                case RAG_CULL_FRUSTUM:
                        c->result = ragg_frustum_find ( ctx->agg, c->type,
                                                        (struct frustum3d*) c->cull_shape );
                        break;
                default:
                        log_mild_err_dbg ( "cull method: %d is not implemented yet",
                                           c->cull_method );
                        break;
                }
        }
        /* notify that all request has processed */
        ctx->n_request = 0;
}
