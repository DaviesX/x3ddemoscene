/* renderable.c: manages all kinds of renderable and connect to local renderable manager interface */
#include <x3d/renderable.h>


/* @todo (davis#2#): <kernel/renderable> implement geometry renderable and simple aggregate */
static const int c_sizeof_renderable[] = {
        [RENDERABLE_GEOMETRY] = sizeof (struct rda_geometry)
};

struct rda_context *create_renderable_context ( void )
{
        return nullptr;
}

void free_renderable_context ( struct rda_context *ctx )
{
}

/* renderable */
struct renderable *create_renderable (
        char *name, enum RENDERABLE_IDR type, float importance, bool is_movable,
        struct material *mtl, struct shader *sha )
{
        return nullptr;
}

void free_renderable ( struct renderable *rda )
{
}

void rda_set_name ( char *name, struct renderable *rda )
{
}

void rda_set_importance ( float importance, struct renderable *rda )
{
}

void rda_set_material ( struct material *mtl, struct renderable *rda )
{
}

void rda_set_shader ( struct shader *sha, struct renderable *rda )
{
}

/* geometry renderable */
void rda_geometry_init_from_data (
        struct point3d *vertex, int num_vert, int *index, int num_tri,
        struct vector3d *normal, struct vector3d *tangent, struct point2d *uv,
        struct matrix4x4 *transform, int ntransform, struct rda_geometry *geo )
{
}

void rda_geometry_refine ( float iteration, struct rda_geometry *geo )
{
}

void rda_geometry_update_vertex ( struct point3d *vertex, int count,
                                  struct rda_geometry *geo )
{
}

void rda_geometry_update_index ( int *index, int count, struct rda_geometry *geo )
{
}

void rda_geometry_update_transform ( int i, struct matrix4x4 *transform,
                                     struct rda_geometry *geo )
{
}

void rda_geometry_fix_nt ( struct rda_geometry *geo )
{
}

struct point3d *rda_geometry_get_vertex ( int *nvertex, struct rda_geometry *geo )
{
        *nvertex = geo->num_vertex;
        return geo->vertex;
}

struct vector3d *rda_geometry_get_normal ( int *nnormal, struct rda_geometry *geo )
{
        *nnormal = geo->num_vertex;
        return geo->normal;
}

struct vector3d *rda_geometry_get_tangent ( int *ntangent, struct rda_geometry *geo )
{
        *ntangent = geo->num_vertex;
        return geo->tangent;
}

struct vector2d *rda_geometry_get_uv ( int *nuv, struct rda_geometry *geo )
{
        *nuv = geo->num_vertex;
        return geo->uv;
}

int *rda_geometry_get_index ( int *nindex, struct rda_geometry *geo )
{
        *nindex = geo->num_tri*3;
        return geo->index;
}

struct matrix4x4 *rda_geometry_get_transform ( int *ntransform, struct rda_geometry *geo )
{
        *ntransform = geo->ntransform;
        return geo->transform;
}

/* aggregate */
void ragg_add_geometry (
        struct rda_geometry *geo, enum RAG_IDR type, struct rda_aggregate *agg )
{
}

void ragg_update ( struct rda_aggregate *agg )
{
}

/* RI Interface */
void rda_context_post_request (
        enum RAG_CULL_IDR cull_type, rda_cullshape_t *shape,
        enum RENDERABLE_IDR rda_type, struct rda_request *request,
        struct rda_context *ctx )
{
}

struct renderable *rda_context_get_i ( int i, enum RENDERABLE_IDR type,
                                       struct rda_context *ctx )
{
        return (struct renderable *) &ctx->rda[type][i*c_sizeof_renderable[type]];
};
