#include <system/log.h>
#include <system/allocator.h>
#include <x3d/renderableaggregaterequest.h>
#include "renderablecontainerimpl.h"


uuid_t rda_context_post_request(struct rda_context* ctx, enum RenderAggregateCullType cull_type, rda_cullshape_t* shape,
                                   enum RenderableType rda_type)
{
        struct rda_request req;
        req.type                = rda_type;
        req.cull_shape          = shape;
        req.cull_method         = cull_type;
        req.id                  = ctx->n_request ++;
        req.has_updated         = false;
        ctx->request[req.id]    = req;
        return req.id;
}

int rda_context_get_n(struct rda_context* ctx, uuid_t request_id)
{
        struct rda_request* req = &ctx->request[request_id];
        if (!req->has_updated) {
                log_guard_frequency_for_next_message(1000);
                log_mild_err_dbg("request(with id: %d) has not been updated", request_id);
                return 0;
        } else {
                return req->n_result;
        }
}

struct rda_instance* rda_context_get_i(struct rda_context* ctx, int i, uuid_t request_id)
{
        return ctx->request[request_id].result[i];
}

void rda_request_init(struct rda_request* req)
{
        zero_obj(req);
        req->cull_shape         = nullptr;
        req->result             = alloc_var(sizeof(*req->result), 1);
        req->n_result           = 0;
        req->has_updated        = false;
}

void rda_request_free(struct rda_request* req)
{
        free_var(req->result);
        zero_obj(req);
}

void rda_context_update(struct rda_context* ctx)
{
        rdacontainer_update(ctx->container);
        /* process all renderable requests */
        int i;
        for ( i = 0; i < ctx->n_request; i ++ ) {
                struct rda_request* c = &ctx->request[i];
                switch (c->cull_method) {
                case RenderAggregateCullFrustum:
                        c->result = rdacontainer_frustum_find(ctx->container, c->type,
                                                             (struct frustum3d*) c->cull_shape,
                                                             &c->n_result);
                        break;
                case RenderAggregateCullCube:
                        c->result = rdacontainer_regional_find(ctx->container, c->type,
                                                              (struct box3d*) c->cull_shape,
                                                              &c->n_result);
                        break;
                default:
                        log_guard_frequency_for_next_message(2000);
                        log_mild_err_dbg("cull method: %d is not implemented yet", c->cull_method);
                        c->n_result = 0;
                        break;
                }
                c->has_updated = true;
        }
        /* notify that all request has processed */
        ctx->n_request = 0;
}
