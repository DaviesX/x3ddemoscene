#ifndef RENDERNODELAYER_H_INCLUDED
#define RENDERNODELAYER_H_INCLUDED

/*
 * structures
 */
struct render_layer {
        struct render_node_ex   _node;
};

/*
 * functions' declaration
 */
struct render_node*  render_node_layer_create(const char* name);


#endif // RENDERNODELAYER_H_INCLUDED
