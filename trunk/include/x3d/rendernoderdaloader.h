#ifndef RENDERNODERDALOADER_H_INCLUDED
#define RENDERNODERDALOADER_H_INCLUDED

/*
 * structures
 */
struct render_rdaloader {
        struct render_node_ex   _node;
        char*                   rdacontext;
        enum RAG_IDR            access;
};

/*
 * functions' declaration
 */
struct render_node*  render_node_rdaloader_create(const char* name, const char* rdacontext, enum RAG_IDR access);
void                 render_node_rdaloader_set_context(struct render_rdaloader* node, const char* context);
void                 render_node_rdaloader_set_access(struct render_rdaloader* node, enum RAG_IDR access);


#endif // RENDERNODERDALOADER_H_INCLUDED
