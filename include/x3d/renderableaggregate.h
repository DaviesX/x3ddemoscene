#ifndef RENDERABLEAGGREGATE_H_INCLUDED
#define RENDERABLEAGGREGATE_H_INCLUDED


#include <container/linkedlist.h>
#include <container/set.h>
#include <x3d/renderable.h>
#include <x3d/light.h>
#include <x3d/bsdf.h>


struct rda_request {
        uuid_t                  id;
        enum RenderableType     type;

	bool			has_updated;

        enum RenderAggregateCullType       cull_method;
        rda_cullshape_t*        cull_shape;

        struct rda_instance**   result;
        int                     n_result;
};

struct rdacontainer;

/*
 * <rda_context> decl
 * handle renderable objects, including materials and lights, and their instances.
 */
struct rda_context {
        struct rdacontainer*            container;              // will handle instances

        set_templ(struct renderable*, void*)    rdas;
        set_templ(struct bsdf_model*, void*)    bsdfs;
        set_templ(struct light*, void*)         lights;

        struct rda_request              request[128];
        int                             n_request;
};

void                    rda_context_init(struct rda_context* ctx, enum RenderAggregateType agg_type);
void                    rda_context_free(struct rda_context* ctx);
void                    rda_context_add_renderable(struct rda_context* ctx, struct renderable* rda);
bool                    rda_context_has_renderable(struct rda_context* ctx, struct renderable* rda);
struct renderable*      rda_context_find_renderable_by_name(struct rda_context* ctx, char* name);
void                    rda_context_remove_renderable(struct rda_context* ctx, struct renderable* rda);
void                    rda_context_remove_renderable_by_name(struct rda_context* ctx, char* name);
void                    rda_context_add_instance (struct rda_context* ctx,
                                                  struct rda_instance* insts,
                                                  enum RenderableType type);
void                    rda_context_add_instance2(struct rda_context* ctx, struct rda_instance* inst);
bool                    rda_context_has_instance(struct rda_context* ctx, struct rda_instance* inst);
void                    rda_context_remove_instance(struct rda_context* ctx, struct rda_instance* inst);
void                    rda_context_clear(struct rda_context* ctx);
int                     rda_context_get_instance_count(struct rda_context* ctx);
int                     rda_context_get_renderable_count(struct rda_context* ctx);

void                    rda_context_add_light(struct rda_context* self, struct light* light);
void                    rda_context_add_material(struct rda_context* self, struct bsdf_model* bsdf);
setptr(struct bsdf_model*)*     rda_context_get_materials(struct rda_context* self);
struct bsdf_model**             rda_context_get_materials2(struct rda_context* self, int* n_materials);
setptr(struct light*)*          rda_context_get_lights(struct rda_context* self);
struct light**                  rda_context_get_lights2(struct rda_context* self, int* n_lights);

#endif // RENDERABLEAGGREGATE_H_INCLUDED
