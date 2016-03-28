#include <misc.h>
#include <system/log.h>
#include <container/set.h>
#include <x3d/renderable.h>
#include <x3d/light.h>
#include <x3d/bsdf.h>
#include <x3d/renderableaggregate.h>
#include <x3d/renderableaggregaterequest.h>
#include "renderablecontainerimpl.h"


/* renderable context */
void rda_context_init(struct rda_context* self, enum RenderAggregateType agg_type)
{
        self->container = rdacontainer_create(agg_type);

        set_init(&self->rdas, 1, nullptr);
        set_init(&self->bsdfs, 1, nullptr);
        set_init(&self->lights, 1, nullptr);

        int i;
        for (i = 0; i < sizeof(self->request)/sizeof(struct rda_request); i ++) {
                rda_request_init(&self->request[i]);
        }
        self->n_request = 0;
}

void rda_context_free(struct rda_context* self)
{
        rdacontainer_free(self->container);

        set_free(&self->rdas);
        set_free(&self->bsdfs);
        set_free(&self->lights);

        int i;
        for ( i = 0;
              i < sizeof(self->request)/sizeof(struct rda_request);
              i ++ ) {
                rda_request_free ( &self->request[i] );
        }
        self->n_request = 0;
}

#define rda_name_hash(_rda)                     (alg_hash_str0(rda_get_name(_rda)))
#define rda_name_cmp(_rda0, _rda1, _data)       (!strcmp(rda_get_name(*_rda0), rda_get_name(*_rda1)))

#define name_hash(_name)                        (alg_hash_str0(_name))
#define name_rda_name_cmp(_name, _rda, _data)   (!strcmp(*_name, rda_get_name(*_rda)))

void rda_context_add_renderable(struct rda_context* self, struct renderable* rda)
{
        set_iter_templ(struct renderable*) iter;
        set_add(&self->rdas, rda, rda_name_hash, rda_name_cmp, &iter);
}

bool rda_context_has_renderable(struct rda_context* self, struct renderable* rda)
{
        set_iter_templ(struct renderable*) iter;
        set_find(&self->rdas, rda, rda_name_hash, rda_name_cmp, &iter);
        return set_iter_has_next(&iter, &self->rdas);
}

struct renderable* rda_context_find_renderable_by_name(struct rda_context* self, char* name)
{
        set_iter_templ(struct renderable*) iter;
        set_find(&self->rdas, name, name_hash, name_rda_name_cmp, &iter);
        if (set_iter_has_next(&iter, &self->rdas)) {
                return set_iter_deref(&iter, &self->rdas);
        } else {
                return nullptr;
        }
}

void rda_context_remove_renderable(struct rda_context* self, struct renderable* rda)
{
        set_iter_templ(struct renderable*) iter;
        set_find(&self->rdas, rda, rda_name_hash, rda_name_cmp, &iter);
        if (set_iter_has_next(&iter, &self->rdas)) {
                set_remove_at(&self->rdas, &iter);
        }
}

void rda_context_remove_renderable_by_name(struct rda_context* self, char* name)
{
        set_iter_templ(struct renderable*) iter;
        set_find(&self->rdas, name, name_hash, name_rda_name_cmp, &iter);
        if (set_iter_has_next(&iter, &self->rdas)) {
                set_remove_at(&self->rdas, &iter);
        }
}

void rda_context_add_instance(struct rda_context* self,
                              struct rda_instance* inst,
                              enum RenderableType type)
{
        rdacontainer_add_instance(self->container, type, inst);
}

void rda_context_add_instance2(struct rda_context* self, struct rda_instance* insts)
{
        rda_context_add_instance(self, insts, rda_get_type(rda_instance_source(insts)));
}

bool rda_context_has_instance(struct rda_context* self, struct rda_instance* inst)
{
        return rdacontainer_has_instance(self->container, inst);
}

void rda_context_remove_instance(struct rda_context* self, struct rda_instance* inst)
{
        rdacontainer_remove_instance(self->container, rda_get_type(rda_instance_source(inst)), inst);
}

void rda_context_clear(struct rda_context* self)
{
        rdacontainer_clear(self->container);
        set_flush(&self->rdas);
        set_flush(&self->bsdfs);
        set_flush(&self->lights);
}

int rda_context_get_instance_count(struct rda_context* self)
{
        return rdacontainer_get_instance_count(self->container);
}

int rda_context_get_renderable_count(struct rda_context* self)
{
        return set_size(&self->rdas);
}

#define light_name_hash(_light)                     (alg_hash_str0(light_get_name(_light)))
#define light_name_cmp(_light0, _light1, _data)     (!strcmp(light_get_name(*_light0), light_get_name(*_light1)))

#define name_light_name_cmp(_name, _light, _data)   (!strcmp(*_name, light_get_name(*_light)))

void rda_context_add_light(struct rda_context* self, struct light* light)
{
        set_iter_templ(struct light*) iter;
        set_add(&self->lights, light, light_name_hash, light_name_cmp, &iter);
}

#define bsdf_name_hash(_bsdf)                           (alg_hash_str0(bsdf_model_get_name(_bsdf)))
#define bsdf_name_cmp(_bsdf0, _bsdf1, _data)            (!strcmp(bsdf_model_get_name(*_bsdf0), bsdf_model_get_name(*_bsdf1)))

#define name_bsdf_name_cmp(_name, _bsdf, _data)         (!strcmp(*_name, bsdf_model_get_name(*_bsdf)))

void rda_context_add_material(struct rda_context* self, struct bsdf_model* bsdf)
{
        set_iter_templ(struct bsdf_model*) iter;
        set_add(&self->bsdfs, bsdf, bsdf_name_hash, bsdf_name_cmp, &iter);
}

setptr(struct bsdf_model*)* rda_context_get_materials(struct rda_context* self)
{
        return (setptr(struct bsdf_model*)*) &self->bsdfs;
}

struct bsdf_model** rda_context_get_materials2(struct rda_context* self, int* n_materials)
{
        *n_materials = set_size(&self->bsdfs);
        set_templ(struct bsdf_model*, void*)* models = cast(models) rda_context_get_materials(self);
        struct bsdf_model** answer = alloc_fix(sizeof(struct bsdf_model*), *n_materials);
        struct bsdf_model* model;
        int i = 0;
        set_for_each(models, model,
                answer[i ++] = model;
        )
        return answer;
}

setptr(struct light*)* rda_context_get_lights(struct rda_context* self)
{
        return (setptr(struct light*)*) &self->lights;
}

struct light** rda_context_get_lights2(struct rda_context* self, int* n_lights)
{
        *n_lights = set_size(&self->lights);
        set_templ(struct light*, void*)* lights = cast(lights) rda_context_get_lights(self);
        struct light** answer = alloc_fix(sizeof(struct light*), *n_lights);
        struct light* light;
        int i = 0;
        set_for_each(lights, light,
                answer[i ++] = light;
        )
        return answer;
}
