#include <math/math.h>
#include <system/log.h>
#include <system/allocator.h>
#include <container/linkedlist.h>
#include "renderablecontainerimpl.h"


static void rdacontainer_init(struct rdacontainer* cont, struct rdacontainer_ops* ops);
static void rdacontainer_free_internal(struct rdacontainer* cont);

static void rdacontainer_linear_init(struct rdacontainer_linear* cont);
static void rdacontainer_linear_update(struct rdacontainer_linear* cont);
static void rdacontainer_linear_free(struct rdacontainer_linear* cont);
static void rdacontainer_linear_add(struct rdacontainer_linear* cont, enum RenderableType type, struct rda_instance* inst);
static void rdacontainer_linear_remove(struct rdacontainer_linear* cont, enum RenderableType type, struct rda_instance* inst);
static void rdacontainer_linear_clear(struct rdacontainer_linear* cont);
static struct rda_instance** rdacontainer_linear_find_frustum(
                struct rdacontainer_linear* cont,
                enum RenderableType type,
                struct frustum3d* f, int* n_instance);
static struct rda_instance** rdacontainer_linear_find_regional(
                struct rdacontainer_linear* self,
                enum RenderableType type,
                struct box3d* f, int* n_instance);


/* linear rdacontainer */
static void rdacontainer_linear_init(struct rdacontainer_linear* cont)
{
        struct rdacontainer_ops ops;
        ops.add = cast(ops.add) rdacontainer_linear_add;
        ops.free = cast(ops.free) rdacontainer_linear_free;
        ops.remove = cast(ops.remove) rdacontainer_linear_remove;
        ops.update = cast(ops.update) rdacontainer_linear_update;
        ops.clear = cast(ops.clear) rdacontainer_linear_clear;
        ops.frustum_find = cast(ops.frustum_find) rdacontainer_linear_find_frustum;
        ops.regional_find = cast(ops.regional_find) rdacontainer_linear_find_regional;
        rdacontainer_init(&cont->_parent, &ops);
        int i;
        for ( i = 0; i < sizeof(cont->insts)/sizeof(char*); i ++ ) {
                cont->insts[i] = alloc_var(sizeof(struct rda_instance*), 1);
                cont->n_insts[i] = 0;
        }
}

static void rdacontainer_linear_update(struct rdacontainer_linear* cont)
{
        /* do nothing */
}

static void rdacontainer_linear_free(struct rdacontainer_linear* cont)
{
        int i;
        for ( i = 0; i < sizeof(cont->insts)/sizeof(char*); i ++ ) {
                free_var ( cont->insts[i] );
        }
        rdacontainer_free_internal((struct rdacontainer*) cont);
}

static void rdacontainer_linear_add(struct rdacontainer_linear* cont, enum RenderableType type, struct rda_instance* inst)
{
        int n = cont->n_insts[type] ++;
        cont->insts[type] = alloc_var_add(cont->insts[type], 1);
        cont->insts[type][n] = inst;
}

static void rdacontainer_linear_remove(struct rdacontainer_linear* cont, enum RenderableType type, struct rda_instance* inst)
{
        /* no remove functionality */
        log_mild_err_dbg ( "linear renderable aggregate has no remove functionality" );
}

static void rdacontainer_linear_clear(struct rdacontainer_linear* cont)
{
        int i;
        for ( i = 0; i < sizeof(cont->insts)/sizeof(char*); i ++ ) {
                alloc_var_flush(cont->insts[i]);
        }
}

static struct rda_instance** rdacontainer_linear_find_frustum(struct rdacontainer_linear* self, 
                                                                  enum RenderableType type,
                                                                  struct frustum3d* f, int* n_instance)
{
        /* @fixme (davis#3#): find intersection against the frustum */
        *n_instance = self->n_insts[type];
        return self->insts[type];
}

static struct rda_instance** rdacontainer_linear_find_regional(struct rdacontainer_linear* self, 
                                                                  enum RenderableType type,
                                                                  struct box3d* f, int* n_instance)
{
        /* @fixme (davis#3#): find intersection against the regional cube */
        *n_instance = self->n_insts[type];
        return self->insts[type];
}

/* aggregate definition */
static void rdacontainer_init(struct rdacontainer* cont, struct rdacontainer_ops* ops)
{
        cont->num_instance = 0;
        cont->num_renderable = 0;
        cont->ops = *ops;
        alg_init(llist, &cont->inst_store, sizeof(struct rda_instance*), 0);
}

static void rdacontainer_free_internal(struct rdacontainer* cont)
{
        alg_free(llist, &cont->inst_store);
        zero_obj(cont);
}

struct rdacontainer* rdacontainer_create(enum RenderAggregateType type)
{
        struct rdacontainer* cont = nullptr;
        /* allocate new object and link its operations */
        switch ( type ) {
        case RenderAggregateLinear:
                cont = alloc_fix(sizeof(struct rdacontainer_linear), 1);
                rdacontainer_linear_init((struct rdacontainer_linear*) cont);
                break;
        case RenderAggregateStaticBVH:
                break;
        case RenderAggregateDynamicGrid:
                break;
        }
        return cont;
}

void rdacontainer_free(struct rdacontainer* cont)
{
        cont->ops.free(cont);
}

void rdacontainer_update(struct rdacontainer* cont)
{
        cont->ops.update(cont);
}

void rdacontainer_clear(struct rdacontainer* cont)
{
        cont->ops.clear(cont);
        cont->num_instance = 0;
}

void rdacontainer_add_instance(struct rdacontainer* cont, enum RenderableType type, struct rda_instance* inst)
{
        cont->ops.add(cont, type, inst);
        alg_llist_push_back(&cont->inst_store, inst);
        cont->num_instance ++;
}

#define cmp_instance_address(_subject, _incoming)       (*_subject == *_incoming)
void rdacontainer_remove_instance(struct rdacontainer* cont, enum RenderableType type, struct rda_instance* inst)
{
        alg_iter(struct rda_instance*) iter;
        alg_llist_find(&cont->inst_store, &inst, iter, cmp_instance_address);
        if (iter != nullptr) {
                cont->ops.remove(cont, type, inst);
                alg_llist_remove(&cont->inst_store, iter);
                cont->num_instance --;
        } else {
                log_mild_err_dbg("couldn't find such renderable instance as: %x", inst);
        }
}

struct rda_instance** rdacontainer_frustum_find(struct rdacontainer* self, enum RenderableType type, 
                                                   struct frustum3d* f, int* n_instance)
{
        return self->ops.frustum_find(self, type, f, n_instance);
}

struct rda_instance** rdacontainer_regional_find(struct rdacontainer* self, enum RenderableType type, 
                                                    struct box3d* b, int* n_instance)
{
        return self->ops.regional_find(self, type, b, n_instance);
}

int rdacontainer_get_instance_count(struct rdacontainer* cont)
{
        return cont->num_instance;
}

int rdacontainer_get_renderable_count(struct rdacontainer* cont)
{
        return cont->num_instance;
}

bool rdacontainer_has_instance(struct rdacontainer* cont, struct rda_instance* inst)
{
        alg_iter(struct rda_instance*) iter;
        alg_llist_find(&cont->inst_store, &inst, iter, cmp_instance_address);
        return iter != nullptr;
}
#undef cmp_instance_address
