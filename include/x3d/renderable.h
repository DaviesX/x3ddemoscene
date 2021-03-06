#ifndef RENDERABLE_H_INCLUDED
#define RENDERABLE_H_INCLUDED

#include <misc.h>
#include <x3d/common.h>
#include <math/math.h>

/*
 * structure
 */
struct mtl_library {
};

/*
 * functions' declaration
 */


enum RenderAggregateCullType {
        RenderAggregateCullNull           = (0),
        RenderAggregateCullCube           = (1 << 1),
        RenderAggregateCullFrustum        = (1 << 2),
        RenderAggregateCullPyramid        = (1 << 3),
	RenderAggregateCullOcclusion      = (1 << 4),
        RenderAggregateCullLOD            = (1 << 5)
};

enum RenderableType {
        RenderableGeometry,
        RenderableSkinned,
        RenderableParticle,
        RenderableVolume,
        RenderableProcedural,
        RenderableTextural
};

#define MAX_RENDERABLE_TYPE           6

enum RenderAggregateType {
        RenderAggregateLinear,
        RenderAggregateStaticBVH,
        RenderAggregateDynamicGrid
};

/*
 * structures
 */
struct material;
struct rda_instance;
struct ragg;
struct renderable;
typedef void rda_cullshape_t;


struct renderable {
        struct {
                void                    (*free)(struct renderable* self);
                struct rda_instance*    (*instance_create)(struct renderable *self, struct matrix4x4 *transform);
                void                    (*instance_free)(struct rda_instance* inst);
        };
        uuid_t                  id;
        char*                   name;
	enum RenderableType     type;
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
        int*                    mater_id;
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


/*
 * functions' declaration
 */
/* renderable */
/* struct renderable*      rda_create(char *name, enum RENDERABLE_IDR type,
                                     float importance, bool is_movable,
                                     int mater_ref);*/
void                    rda_init(struct renderable* self,
                                  char *name, enum RenderableType type, float importance,
                                  bool is_movable, int mater_ref);
void                    rda_free(struct renderable *rda);
void                    rda_set_name(struct renderable *rda, char *name);
void                    rda_set_importance(struct renderable *rda, float importance);
void                    rda_set_material(struct renderable *rda, int mater_ref);
char*                   rda_get_name(struct renderable *self);
enum RenderableType	rda_get_type(struct renderable* self);
int                     rda_get_material_reference(struct renderable* self);
struct rda_instance*    rda_instance_create(struct renderable *rda, struct matrix4x4 *transform);
void                    rda_instance_free(struct rda_instance *inst);
struct renderable*      rda_instance_source(struct rda_instance* inst);

/* geometry renderable */
struct rda_geometry* rda_geometry_create(char *name, float importance, bool is_movable, int mater_ref);
void rda_geometry_init_from_data(struct rda_geometry *geo,
                                    struct point3d* vertex, int num_vert, int* index, int num_tri,
                                    struct vector3d* normal, struct vector3d* tangent, struct point2d* uv, 
                                    struct matrix4x4* transform);
void rda_geometry_refine(struct rda_geometry* geo, float iteration);
void rda_geometry_update_vertex(struct rda_geometry* geo, struct point3d* vertex, int count);
void rda_geometry_update_index(struct rda_geometry* geo, int* index, int count);
void rda_geometry_set_material_id_list(struct rda_geometry* self, int* mater_id);
void rda_geometry_set_transform(struct rda_geometry* geo, struct matrix4x4 *transform);
void rda_geometry_fix_nt(struct rda_geometry* geo);

struct point3d*         rda_geometry_get_vertex(struct rda_geometry* geo, int *nvertex);
struct vector3d*        rda_geometry_get_normal(struct rda_geometry* geo, int* nnormal);
struct vector3d*        rda_geometry_get_tangent(struct rda_geometry* geo, int* ntangent);
struct vector2d*        rda_geometry_get_uv(struct rda_geometry* geo, int* nuv);
int*                    rda_geometry_get_material_id_list(struct rda_geometry* self, int* nmater) ;
int*                    rda_geometry_get_index(struct rda_geometry* geo, int* nindex);


#endif // RENDERABLE_H_INCLUDED
