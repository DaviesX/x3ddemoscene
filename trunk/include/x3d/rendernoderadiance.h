#ifndef RENDERNODERADIANCE_H_INCLUDED
#define RENDERNODERADIANCE_H_INCLUDED


#include <x3d/renderer.h>

enum RenderPipeType {
        RenderPipeForward,
        RenderPipeForwardPlus,
        RenderPipeWhitted,
        RenderPipeDirectLighting,
        RenderPipeMetropolis,
        RenderPipeImageSpace
};

/*
 * structures
 */
struct render_radiance {
        struct render_node_ex   _node;
        enum RenderPipeType     pipe;
};

/*
 * functions' declaration
 */
struct render_node*     render_node_radiance_create(const char* name, enum RenderPipeType pipe);
void                    render_node_radiance_set_pipe(struct render_radiance* node, enum RenderPipeType pipe);
enum RenderPipeType     render_node_radiance_get_pipe(const struct render_radiance* node);
const int               render_node_radiance_get_input_slot(enum RenderNodeType type);


#endif // RENDERNODERADIANCE_H_INCLUDED
