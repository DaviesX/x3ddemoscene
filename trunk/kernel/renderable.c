/* renderable.c: manages all kinds of renderable and connect to local renderable manager interface */
#include <misc.h>
#include <math/math.h>
#include <system/log.h>
#include <system/allocator.h>
#include <x3d/renderable.h>


/* @todo (davis#2#): <kernel/renderable> implement geometry renderable and simple aggregate */
static const int c_sizeof_renderable[] = {
        [RenderableGeometry] = sizeof (struct rda_geometry)
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

struct rda_geometry* rda_geometry_create ( char *name, float importance, bool is_movable, int mater_ref )
{
        struct rda_geometry* geo = alloc_obj ( geo );
        rda_init(&geo->_parent, name, RenderableGeometry, importance, is_movable, mater_ref);
        return geo;
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
        /* normal(optional) */
        if ( normal != nullptr ) {
                geo->normal     = alloc_fix ( sizeof(struct vector3d), num_vert );
                memcpy ( geo->normal, normal, sizeof(struct vector3d)*num_vert );
        }
        /* tangent(optional) */
        if ( tangent != nullptr ) {
                geo->tangent     = alloc_fix ( sizeof(struct vector3d), num_vert );
                memcpy ( geo->tangent, tangent, sizeof(struct vector3d)*num_vert );
        }
        /* uv(optional) */
        if ( uv != nullptr ) {
                geo->uv         = alloc_fix ( sizeof(struct vector2d), num_vert );
                memcpy ( geo->uv, uv, sizeof(struct vector2d)*num_vert );
        }
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
        *nvertex = geo->num_vertex;
        return geo->vertex;
}

struct vector3d* rda_geometry_get_normal ( struct rda_geometry* geo, int* nnormal )
{
        *nnormal = geo->num_vertex;
        return geo->normal;
}

struct vector3d* rda_geometry_get_tangent ( struct rda_geometry* geo, int* ntangent )
{
        *ntangent = geo->num_vertex;
        return geo->tangent;
}

struct vector2d* rda_geometry_get_uv ( struct rda_geometry* geo, int* nuv )
{
        *nuv = geo->num_vertex;
        return geo->uv;
}

int* rda_geometry_get_index ( struct rda_geometry* geo, int* nindex )
{
        *nindex = geo->num_tri*3;
        return geo->index;
}

/* renderable */
static const struct rda_operation {
        void (*free) ( struct renderable* self );
        struct renderable* (*make_copy) ( struct renderable* self );
        void (*get_bound) ( struct renderable* self, struct matrix4x4* t, struct box3d* b );
} cRdaOps[] = {
        [RenderableGeometry].free = cast(cRdaOps->free)                rda_geometry_free,
        [RenderableGeometry].make_copy =  cast(cRdaOps->make_copy)     rda_geometry_make_copy,
        [RenderableGeometry].get_bound = cast(cRdaOps->get_bound)      rda_geometry_get_bound
};

void rda_init(struct renderable* self,
               char *name, enum RenderableType type, float importance,
               bool is_movable, int mater_ref)
{
        self->id         = alg_gen_uuid ( );
        self->importance = importance;
        self->name       = alg_alloc_string ( name );
        self->is_dirty   = true;
        self->is_movable = is_movable;
        self->mater_ref  = mater_ref;
        self->insts      = alloc_var ( sizeof(struct rda_instance*), 1 );
        self->type       = type;
}

static void h_renderable_free ( struct renderable* rda )
{
        free_var ( rda->insts );
        free_fix ( rda->name );
        zero_obj ( rda );
}
/*
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
*/
void rda_free ( struct renderable *rda )
{
        h_renderable_free ( rda );
        cRdaOps[rda->type].free ( rda );
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

enum RenderableType rda_get_type ( struct renderable* rda )
{
        return rda->type;
}

int rda_get_material_reference(struct renderable* self)
{
        return self->mater_ref;
}

struct rda_instance* rda_instance_create ( struct renderable *rda, struct matrix4x4 *transform )
{
        struct rda_instance* insts = alloc_obj ( insts );
        insts->transform        = *transform;
        insts->i                = rda->ninsts;
        insts->rda_link         = rda;
        cRdaOps[rda->type].get_bound ( rda, transform, &insts->bound );

        rda->insts = alloc_var_add ( rda->insts, 1 );
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

