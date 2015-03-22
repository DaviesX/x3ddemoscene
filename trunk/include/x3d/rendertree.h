#ifndef RENDERTREE_H_INCLUDED
#define RENDERTREE_H_INCLUDED


#include <x3d/renderable.h>

struct render_tree;
struct render_node;
struct render_node_ops;

/*
 * class render_node
 */
enum LIGHT_MODEL_IDR {
        LIGHT_MODE_DIRECT     = 0X1,
        LIGHT_MODE_SHADOW     = 0X2,
        LIGHT_MODE_LIGHTMAP   = 0X4,
        LIGHT_MODE_SH_PROBE   = 0X8,
        LIGHT_MODE_SVO        = 0X10
};

enum GEOMETRY_MODEL_IDR {
        GEOMETRY_MODEL_WIREFRAME,
        GEOMETRY_MODEL_SOLID
};

enum RENDER_NODE_IDR {
        RENDER_NODE_ROOT,
        RENDER_NODE_LAYER,
        RENDER_NODE_RADIANCE,
        RENDER_NODE_OUTPUT,
        RENDER_NODE_RDACONTEXT
};

struct render_node_ops {
        bool (*free_self)       (struct render_node* self);
        bool (*verify_self)     (struct render_tree* tree, struct render_node* self);
        bool (*insert_input)    (struct render_node* self, struct render_node* input);
        bool (*remove_input)    (struct render_node* self, struct render_node* input);
};

struct render_node {
        struct render_node_ops  ops;
        char*                   name;
        enum RENDER_NODE_IDR    type;
        struct render_node**    input;
        int                     n_input;
        struct render_node**    output;
        int                     i_output;
        int                     n_output;
};


struct render_output {
        struct render_node      _node;
        char*                   probe;
};
__dlexport struct render_node*  render_node_output_create(const char* name, const char* probe);
__dlexport void                 render_node_output_set_probe(struct render_output* node, const char* probe);


struct render_layer {
        struct render_node      _node;
};
__dlexport struct render_node*  render_node_layer_create(const char* name);


struct render_rdacontext {
        struct render_node      _node;
        char*                   rdacontext;
        enum RAG_IDR            access;
};
__dlexport struct render_node*  render_node_rdacontext_create(const char* name, const char* rdacontext, enum RAG_IDR access);
__dlexport void                 render_node_rdacontext_set_context(struct render_rdacontext* node, const char* context);
__dlexport void                 render_node_rdacontext_set_access(struct render_rdacontext* node,
                                                                  enum RAG_IDR access);


enum RENDER_PIPE_IDR {
        RENDER_PIPE_FORWARD,
        RENDER_PIPE_FORWARD_PLUS,
        RENDER_PIPE_WHITTED,
        RENDER_PIPE_DIRECT_LIGHTING,
        RENDER_PIPE_METROPOLIS,
        RENDER_PIPE_IMAGE_SPACE
};
struct render_radiance {
        struct render_node      _node;
        enum RENDER_PIPE_IDR    pipe;
};
__dlexport struct render_node*  render_node_radiance_create(const char* name, enum RENDER_PIPE_IDR pipe);
__dlexport void                 render_node_radiance_set_pipe(struct render_radiance* node, enum RENDER_PIPE_IDR pipe);


struct render_post_ao {
        struct render_node      _node;
};

struct render_filmic_hdr {
        struct render_node      _node;
        float     A;// = 0.15f;
        float     B;// = 0.50f;
        float     C;// = 0.10f;
        float     D;// = 0.20f;
        float     E;// = 0.02f;
        float     F;// = 0.30f;
        float     W;// = 11.2f;
        float           key;
        float           exp;
};

struct render_post_aa {
};

struct render_dof {
};

struct render_color_grading {
};

struct render_mask {
};

struct render_blending {
};

struct render_gamma {
};


/*
 * class render_tree
 */
enum RENDERER_THREAD_STATE_IDR {
        /* Renderer should not handle threading from these constants itself,
         * this are the states telling which situation
         * the renderer is being created with */
        RENDERER_THREAD_MUTUAL = 0X0,
        RENDERER_THREAD_SEPARATED = 0X1,

        /* These are what renderer expected to behave internally */
        RENDERER_THREAD_SINGLE = 0X2,
        RENDERER_THREAD_MULTIPLE = 0X4,
};

enum RENDER_SPEC_IDR {
        RENDER_SPEC_SW_BUILTIN,
        RENDER_SPEC_HW_OPENGL,
        RENDER_SPEC_HW_DIRECTX
};

enum RENDER_ENVIRONMENT {
        RENDER_ENV_VOID,
        RENDER_ENV_SPEC,
        RENDER_ENV_PROBE,
        RENDER_ENV_RDA,
        RENDER_ENV_RENDER_OUT,
        RENDER_ENV_THREAD,
        RENDER_ENV_ANTIALIAS,
        RENDER_ENV_FILTERING
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
                                                                enum RENDER_ENVIRONMENT type,
                                                                const char* var_name, void* var);
__dlexport void                 render_tree_clear_environment(struct render_tree* tree);
__dlexport bool                 render_tree_compile(struct render_tree* tree, struct render_bytecode* bytecode);
__dlexport bool                 render_tree_visit(struct render_tree* tree, struct render_tree_visitor* visitor);


#endif // RENDERTREE_H_INCLUDED
