#include <x3d/renderableaggregate.h>
#include <x3d/renderableaggregaterequest.h>
#include <logout.h>
#include "renderablecontainerimpl.h"


/* renderable context */
void rda_context_init(struct rda_context* ctx, enum RAG_IDR agg_type)
{
        ctx->container = rdacontainer_create(agg_type);
        ctx->num_renderable = 0;
        int i;
        for ( i = 0;
              i < sizeof(ctx->request)/sizeof(struct rda_request);
              i ++ ) {
                rda_request_init ( &ctx->request[i] );
        }
        ctx->n_request = 0;
}

void rda_context_free(struct rda_context* ctx)
{
        rdacontainer_free(ctx->container);
        int i;
        for ( i = 0;
              i < sizeof(ctx->request)/sizeof(struct rda_request);
              i ++ ) {
                rda_request_free ( &ctx->request[i] );
        }
        ctx->n_request = 0;
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
                              enum RENDERABLE_IDR type)
{
        rdacontainer_add_instance(ctx->container, type, inst);
}

void rda_context_add_instance2(struct rda_context* ctx, struct rda_instance* insts)
{
        rda_context_add_instance(ctx, insts, rda_get_type(rda_instance_source(insts)));
}

bool rda_context_has_instance(struct rda_context* ctx, struct rda_instance* inst)
{
        log_mild_err_dbg("have not implemented yet");
        return false;
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
