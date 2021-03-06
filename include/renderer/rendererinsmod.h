#ifndef RENDERERIMPL_H_INCLUDED
#define RENDERERIMPL_H_INCLUDED

#include <x3d/renderer.h>


/*
 * functions' declaration
 */
typedef struct render_node_ex_impl* (*f_Render_Node_Creator) (struct render_node_ex* parent);

void renderer_install_render_node_factory(int id, enum RenderNodeType type, f_Render_Node_Creator creator);
void renderer_remove_render_node_factory(int id, enum RenderNodeType type);



#endif // RENDERERIMPL_H_INCLUDED
