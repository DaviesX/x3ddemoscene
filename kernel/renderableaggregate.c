#include <x3d/renderableaggregate.h>
#include <x3d/renderableaggregaterequest.h>
#include <system/log.h>
#include "renderablecontainerimpl.h"


/* renderable context */
void rda_context_init(struct rda_context* self, enum RenderAggregateType agg_type)
{
        self->container = rdacontainer_create(agg_type);

        self->bsdfs = alloc_var(sizeof(struct bsdf_model*), 1);
        self->lights = alloc_var(sizeof(struct light*), 1);
        self->n_bsdfs = 0;
        self->n_lights = 0;
        self->num_renderable = 0;

        int i;
        for (i = 0; i < sizeof(self->request)/sizeof(struct rda_request); i ++) {
                rda_request_init(&self->request[i]);
        }
        self->n_request = 0;
}

void rda_context_free(struct rda_context* self)
{
        rdacontainer_free(self->container);

        free_var(self->bsdfs);
        free_var(self->lights);

        int i;
        for ( i = 0;
              i < sizeof(self->request)/sizeof(struct rda_request);
              i ++ ) {
                rda_request_free ( &self->request[i] );
        }
        self->n_request = 0;
}

void rda_context_add_renderable(struct rda_context* ctx, struct renderable* rda)
{
        ctx->num_renderable ++;
}

bool rda_context_has_renderable(struct rda_context* ctx, struct renderable* rda)
{
        log_mild_err_dbg("have not implemented yet");
        return false;
}

struct renderable* rda_context_find_renderable_by_name(struct rda_context* ctx, char* name)
{
        log_mild_err_dbg("have not implemented yet");
        return nullptr;
}

void rda_context_remove_renderable(struct rda_context* ctx, struct renderable* rda)
{
        log_mild_err_dbg("have not implemented yet");
        ctx->num_renderable --;
}

void rda_context_remove_renderable_by_name(struct rda_context* ctx, char* name)
{
        log_mild_err_dbg("have not implemented yet");
        ctx->num_renderable --;
}

void rda_context_add_instance(struct rda_context* ctx,
                              struct rda_instance* inst,
                              enum RenderableType type)
{
        rdacontainer_add_instance(ctx->container, type, inst);
}

void rda_context_add_instance2(struct rda_context* ctx, struct rda_instance* insts)
{
        rda_context_add_instance(ctx, insts, rda_get_type(rda_instance_source(insts)));
}

bool rda_context_has_instance(struct rda_context* ctx, struct rda_instance* inst)
{
        return rdacontainer_has_instance(ctx->container, inst);
}

void rda_context_remove_instance(struct rda_context* ctx, struct rda_instance* inst)
{
        rdacontainer_remove_instance(ctx->container, rda_get_type(rda_instance_source(inst)), inst);
}

void rda_context_clear(struct rda_context* ctx)
{
        rdacontainer_clear(ctx->container);
        ctx->num_renderable = 0;
}

int rda_context_get_instance_count(struct rda_context* ctx)
{
        return rdacontainer_get_instance_count(ctx->container);
}

int rda_context_get_renderable_count(struct rda_context* ctx)
{
        return ctx->num_renderable;
}

void rda_context_add_light(struct rda_context* self, struct light* light)
{
        self->lights = alloc_var_add(self->lights, 1);
        self->lights[self->n_lights ++] = light;
}

void rda_context_add_material(struct rda_context* self, struct bsdf_model* bsdf)
{
        self->bsdfs = alloc_var_add(self->bsdfs, 1);
        self->bsdfs[self->n_bsdfs ++] = bsdf;
}

struct bsdf_model** rda_context_get_materials(struct rda_context* self, int *n_materials)
{
        *n_materials = self->n_bsdfs;
        return self->bsdfs;
}

struct light** rda_context_get_lights(struct rda_context* self, int* n_lights)
{
        *n_lights = self->n_lights;
        return self->lights;
}
