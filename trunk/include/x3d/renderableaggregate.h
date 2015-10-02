#ifndef RENDERABLEAGGREGATE_H_INCLUDED
#define RENDERABLEAGGREGATE_H_INCLUDED


#include <container/linkedlist.h>
#include <x3d/renderable.h>

struct rda_request {
        uuid_t                  id;
        enum RENDERABLE_IDR     type;
        enum RAG_CULL_IDR       cull_method;
        rda_cullshape_t*        cull_shape;

        struct rda_instance**   result;
        int                     n_result;
};

struct rdacontainer;

struct rda_context {
        struct rdacontainer*            container;
        d_alg_llist(struct renderable*) rdalist;
        int                             num_renderable;
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
                                                  enum RENDERABLE_IDR type);
void                    rda_context_add_instance2(struct rda_context* ctx, struct rda_instance* inst);
bool                    rda_context_has_instance(struct rda_context* ctx, struct rda_instance* inst);
void                    rda_context_remove_instance(struct rda_context* ctx, struct rda_instance* inst);
void                    rda_context_clear(struct rda_context* ctx);
int                     rda_context_get_instance_count(struct rda_context* ctx);
int                     rda_context_get_renderable_count(struct rda_context* ctx);


#endif // RENDERABLEAGGREGATE_H_INCLUDED
