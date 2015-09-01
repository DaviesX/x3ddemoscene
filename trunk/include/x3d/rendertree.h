#ifndef RENDERTREE_H_INCLUDED
#define RENDERTREE_H_INCLUDED


#include <x3d/renderable.h>
#include <x3d/rendertreeenvconsts.h>

/*
 * forward declaration's
 */
struct render_tree;
struct render_node;
struct render_node_ops;

/*
 * consts
 */
enum RenderNodeType {
        // RenderNodeNull,
        RenderNodeRoot,
        RenderNodeRenderableLoader,
        RenderNodeLayerLoader,
        RenderNodeProbeLoader,
        RenderNodeRadiance,
        RenderNodeOutput,
        c_NumRenderNode
};

struct render_node_ops {
        bool (*free_self)               (struct render_node* self);
        bool (*verify_self)             (struct render_tree* tree, struct render_node* self);
        bool (*insert_input)            (struct render_node* self, struct render_node* input);
        bool (*remove_input)            (struct render_node* self, struct render_node* input);
};

struct render_node {
        struct render_node_ops  ops;
        char*                   name;
        enum RenderNodeType     type;
        struct render_node**    input;
        int                     n_input;
        struct render_node**    output;
        int                     i_output;
        int                     n_output;
};

enum RENDER_OP {
        RENDER_OP_NULL,
        RENDER_OP_LOAD_RDA,             /* LDRDA        m_dest, rda, type */
        RENDER_OP_LOAD_IMG,             /* LDIMG        m_dest, img, type */
        RENDER_OP_LOAD_PROBE,           /* LDPRO        m_dest, probe */
        RENDER_OP_MOVE,                 /* MOV          r_dest, r_src */
        RENDER_OP_RADIANCE,             /* RAD          r_dest, m_rda, pipe */
        RENDER_OP_COMPOSITE,            /* COMP         r_dest, type, r_src  */
        RENDER_OP_OUTPUT                /* OUT          m_probe, r_src */
};


struct render_bytecode {
        char    instr[1024];
};

struct render_tree_visitor {
        void    (*this_node) (struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr);
        void    (*backtrack) (struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr);
        void*   dataptr;
};

void render_tree_visitor_init(struct render_tree_visitor* visitor,
                              void (*this_node) (struct render_node* node,
                                                 struct render_node* input[],
                                                 struct render_node* output[],
                                                 void* dataptr),
                              void* dataptr);
void render_tree_visitor_init2(struct render_tree_visitor* visitor,
                               void (*this_node) (struct render_node* node,
                                                  struct render_node* input[],
                                                  struct render_node* output[],
                                                  void* dataptr),
                                void (*backtrack) (struct render_node* node,
                                                   struct render_node* input[],
                                                   struct render_node* output[],
                                                   void* dataptr),
                                void* dataptr);

struct render_tree {
        struct render_node*     node_tree;
        struct {
                char*           name;
                void*           ptr;
        }                       var[20][50];
        int                     n_var[20];
};
/* struct render_node's */
__dlexport void                 render_node_init(struct render_node* node,
                                                 bool (*free_self)       (struct render_node* self),
                                                 bool (*verify_self)     (struct render_tree* tree, struct render_node* self),
                                                 bool (*insert_input)    (struct render_node* self, struct render_node* input),
                                                 bool (*remove_input)    (struct render_node* self, struct render_node* input),
                                                 enum RenderNodeType type, const char* name, int n_input, int n_output);
__dlexport void                 render_node_free(struct render_node* node);
__dlexport void                 render_node_set_input(struct render_node* node, struct render_node* input, int i);
__dlexport void                 render_node_remove_input(struct render_node* node, struct render_node* input);
__dlexport enum RenderNodeType  render_node_get_type(struct render_node* node);
__dlexport const char*          render_node_get_name(struct render_node* node);
__dlexport void                 render_node_copy_link(struct render_node* src, struct render_node* dest);

__dlexport bool                 render_tree_check_environment(struct render_tree* tree, enum RenderEnvironment type, char* name);

/* struct render_tree's */
__dlexport void                 render_tree_init(struct render_tree* tree);
__dlexport void                 render_tree_free(struct render_tree* tree);
__dlexport struct render_node*  render_tree_create_root(struct render_tree* tree);
__dlexport struct render_node*  render_tree_get_node(struct render_tree* tree, const char* name);
__dlexport bool                 render_tree_insert_node(struct render_tree* tree,
                                                        struct render_node* parent,
                                                        struct render_node* node);
__dlexport bool                 render_tree_replace_node(struct render_tree* tree, struct render_node* node);
__dlexport bool                 render_tree_remove_node(struct render_tree* tree,
                                                        struct render_node* parent,
                                                        struct render_node* node);
__dlexport void                 render_tree_declare_environment(struct render_tree* tree,
                                                                enum RenderEnvironment type,
                                                                const char* var_name, void* var);
__dlexport void*                render_tree_retrieve_environment(struct render_tree* tree, enum RenderEnvironment type);
__dlexport void                 render_tree_clear_environment(struct render_tree* tree);
__dlexport bool                 render_tree_visit(struct render_tree* tree, struct render_tree_visitor* visitor);


#endif // RENDERTREE_H_INCLUDED
