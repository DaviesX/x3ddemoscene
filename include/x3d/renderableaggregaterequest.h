#ifndef RENDERABLEAGGREGATEREQUEST_H_INCLUDED
#define RENDERABLEAGGREGATEREQUEST_H_INCLUDED


#include <x3d/renderableaggregate.h>

void                    rda_request_init ( struct rda_request* req );
void                    rda_request_free ( struct rda_request* req );
void                    rda_context_update ( struct rda_context* ctx );
uuid_t                  rda_context_post_request ( struct rda_context* ctx,
                                                   enum RenderAggregateCullType cull_type, rda_cullshape_t* shape,
                                                   enum RenderableType rda_type );
int                     rda_context_get_n ( struct rda_context* ctx, uuid_t request_id );
struct rda_instance*    rda_context_get_i ( struct rda_context* ctx, int i, uuid_t request_id );


#endif // RENDERABLEAGGREGATEREQUEST_H_INCLUDED
