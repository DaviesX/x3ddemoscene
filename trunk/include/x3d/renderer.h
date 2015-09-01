#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include <misc.h>
#include <x3d/common.h>
#include <x3d/rendertree.h>

/*
 * forward declarations
 */
struct render_tree;
struct render_node;
struct render_node_ex;
struct render_node_ex_impl;
struct symbol_set;

/*
 * structures
 */

typedef bool (*f_Render_NodeExImpl_Is_Compatiable) (struct render_node_ex_impl* self, struct render_tree* tree);
typedef void (*f_Render_NodeExImpl_Compute) (struct render_node_ex_impl* self,
                                             const struct render_node_ex_impl* input[],
                                             const struct render_node_ex_impl* output[]);
typedef void* (*f_Render_NodeExImpl_Get_Result) (struct render_node_ex_impl* self);
typedef void (*f_Render_NodeExImpl_Free) (struct render_node_ex_impl* self);
struct render_node_ex_ops {
        f_Render_NodeExImpl_Is_Compatiable      f_is_compatible;
        f_Render_NodeExImpl_Compute             f_compute;
        f_Render_NodeExImpl_Get_Result          f_get_result;
        f_Render_NodeExImpl_Free                f_free;
};

struct render_node_ex_impl {
        struct render_node_ex_ops       ops;
        struct render_node_ex*          ref;
};

struct render_node_ex {
        struct render_node              _parent;
        enum RenderNodeType             type;
        struct render_node_ex_impl*     metainst[32];
};


/*
 * functions' declaration
 */
/* struct render_node_ex's */
void                            render_node_ex_init(struct render_node_ex* self, enum RenderNodeType type);
void                            render_node_ex_free(struct render_node_ex* self);
struct render_node_ex*          render_node_ex_impl_get_ex(struct render_node_ex_impl* self);
struct render_node_ex_impl*     render_node_ex_get_ex_impl(struct render_node_ex* self, int id);

/* ABIs */
typedef void (*f_Rendererinsmod) ();
typedef void (*f_Rendererrmmod) ();

void            renderer_kernel_init();
void            renderer_kernel_free();
bool            renderer_import(struct symbol_set* symbols);

/* renderer's */
void            renderer_render(struct render_tree* tree);


#endif // RENDERER_H_INCLUDED
