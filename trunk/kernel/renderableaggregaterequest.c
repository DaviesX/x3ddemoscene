#include <x3d/renderableaggregaterequest.h>


uuid_t rda_context_post_request ( struct rda_context* ctx,
                                  enum RAG_CULL_IDR cull_type, rda_cullshape_t* shape,
                                  enum RENDERABLE_IDR rda_type )
{
        struct rda_request req;
        req.cull_shape = shape;
        req.cull_method = cull_type;
        req.id = ctx->n_request ++;
        ctx->request[req.id] = req;
        return req.id;
}

int rda_context_get_n ( struct rda_context* ctx, uuid_t request_id )
{
        return ctx->request[request_id].n_result;
}

struct rda_instance* rda_context_get_i ( struct rda_context* ctx, int i, uuid_t request_id )
{
        return ctx->request[request_id].result[i];
}

void rda_request_init ( struct rda_request* req )
{
        req->cull_shape = nullptr;
        req->result = alloc_var ( sizeof(struct rda_request*), 1 );
        req->n_result = 0;
}

void rda_request_free ( struct rda_request* req )
{
        free_var ( req->result );
        zero_obj ( req );
}
