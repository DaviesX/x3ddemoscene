#include <math/math.h>
#include <system/log.h>
#include <system/allocator.h>
#include "renderablecontainerimpl.h"


static void rdacontainer_init(struct rdacontainer* cont, struct rdacontainer_ops* ops);
static void rdacontainer_free_internal(struct rdacontainer* cont);

static void rdacontainer_linear_init(struct rdacontainer_linear* cont);
static void rdacontainer_linear_update(struct rdacontainer_linear* cont);
static void rdacontainer_linear_free(struct rdacontainer_linear* cont);
static void rdacontainer_linear_add(struct rdacontainer_linear* cont, enum RENDERABLE_IDR type, struct rda_instance* inst);
static void rdacontainer_linear_remove(struct rdacontainer_linear* cont, enum RENDERABLE_IDR type, struct rda_instance* inst);
static void rdacontainer_linear_clear(struct rdacontainer_linear* cont);
static struct rda_instance** rdacontainer_linear_find_frustum(
                struct rdacontainer_linear* cont,
                enum RENDERABLE_IDR type,
                struct frustum3d* f);


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
        rdacontainer_init(&cont->_parent, &ops);
        int i;
        for ( i = 0; i < sizeof(cont->insts)/sizeof(char*); i ++ ) {
                cont->insts[i] = alloc_var ( sizeof(struct rda_instance*), 1 );
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

static void rdacontainer_linear_add(struct rdacontainer_linear* cont, enum RENDERABLE_IDR type, struct rda_instance* inst)
{
        int n = get_var_len(cont->insts[type]);
        cont->insts[type] = add_var ( cont->insts[type], 1 );
        cont->insts[type][n] = inst;
}

static void rdacontainer_linear_remove(struct rdacontainer_linear* cont, enum RENDERABLE_IDR type, struct rda_instance* inst)
{
        /* no remove functionality */
        log_mild_err_dbg ( "linear renderable aggregate has no remove functionality" );
}

static void rdacontainer_linear_clear(struct rdacontainer_linear* cont)
{
        int i;
        for ( i = 0; i < sizeof(cont->insts)/sizeof(char*); i ++ ) {
                flush_var(cont->insts[i]);
        }
}

static struct rda_instance** rdacontainer_linear_find_frustum(
                struct rdacontainer_linear* cont,
                enum RENDERABLE_IDR type,
                struct frustum3d* f)
{
        /* @fixme (davis#1#): find intersection against the frustum */
        return cont->insts[type];
}

/* aggregate definition */
static void rdacontainer_init(struct rdacontainer* cont, struct rdacontainer_ops* ops)
{
        cont->num_instance = 0;
        cont->num_renderable = 0;
        cont->ops = *ops;
        create_alg_llist(&cont->inst_store, sizeof(struct rda_instance*), 0);
}

static void rdacontainer_free_internal(struct rdacontainer* cont)
{
        free_alg_llist(&cont->inst_store);
        zero_obj(cont);
}

struct rdacontainer* rdacontainer_create(enum RAG_IDR type)
{
        struct rdacontainer* cont = nullptr;
        /* allocate new object and link its operations */
        switch ( type ) {
        case RAG_LINEAR:
                cont = alloc_fix(sizeof(struct rdacontainer_linear), 1);
                rdacontainer_linear_init((struct rdacontainer_linear*) cont);
                break;
        case RAG_STATIC_BVH:
                break;
        case RAG_DYNAMIC_GRID:
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

void rdacontainer_add_instance(struct rdacontainer* cont, enum RENDERABLE_IDR type, struct rda_instance* inst)
{
        cont->ops.add(cont, type, inst);
        alg_llist_push_back(inst, &cont->inst_store);
        cont->num_instance ++;
}

#define cmp_instance_address(_subject, _incoming)       (*_subject == *_incoming)
void rdacontainer_remove_instance(struct rdacontainer* cont, enum RENDERABLE_IDR type, struct rda_instance* inst)
{
        alg_iter(struct rda_instance*) iter;
        alg_llist_find(&inst, iter, cmp_instance_address, &cont->inst_store);
        if (iter != nullptr) {
                cont->ops.remove(cont, type, inst);
                alg_llist_remove(iter, &cont->inst_store);
                cont->num_instance --;
        } else {
                log_mild_err_dbg("couldn't find such renderable instance as: %x", inst);
        }
}

struct rda_instance** rdacontainer_frustum_find(struct rdacontainer* cont, enum RENDERABLE_IDR type, struct frustum3d* f)
{
        return cont->ops.frustum_find ( cont, type, f );
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
        alg_llist_find(&inst, iter, cmp_instance_address, &cont->inst_store);
        return iter != nullptr;
}
#undef cmp_instance_address
