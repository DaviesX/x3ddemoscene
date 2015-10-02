#ifndef RENDERNODERDALOADER_H_INCLUDED
#define RENDERNODERDALOADER_H_INCLUDED


#include <x3d/renderer.h>


/*
 * structures
 */
struct render_rdaloader {
        struct render_node_ex   _node;
        char*                   rdacontext;
        enum RenderAggregateType            access;
};

/*
 * functions' declaration
 */
struct render_node*  render_node_rdaloader_create(const char* name, const char* rdacontext, enum RenderAggregateType access);
void                 render_node_rdaloader_set_context(struct render_rdaloader* node, const char* context);
void                 render_node_rdaloader_set_access(struct render_rdaloader* node, enum RenderAggregateType access);

const char*          render_node_rdaloader_get_context(const struct render_rdaloader* node);


#endif // RENDERNODERDALOADER_H_INCLUDED
