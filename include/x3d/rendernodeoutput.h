#ifndef RENDERNODEOUTPUT_H_INCLUDED
#define RENDERNODEOUTPUT_H_INCLUDED

#include <x3d/rendertree.h>
#include <x3d/renderer.h>

/*
 * structures
 */
struct render_output {
        struct render_node_ex   _parent;
        char*                   probe;
};

/*
 * functions' declaration
 */
struct render_node_ex*  render_node_output_create(const char* name, const char* probe);
void                    render_node_output_set_probe(struct render_output* self, const char* probe);
const int               render_node_output_get_input_slot();

const char*             render_node_output_get_probe(const struct render_output* self);


#endif // RENDERNODEOUTPUT_H_INCLUDED
