#ifndef RENDERNODEDUMMYIMPL_H_INCLUDED
#define RENDERNODEDUMMYIMPL_H_INCLUDED

#include <x3d/rendertree.h>
#include <x3d/renderer.h>

struct dummy_root_node_impl {
        struct render_node_ex_impl      _parent;
};

static void dummy_root_node_compute(struct render_node_ex_impl* self,
                                    const struct render_node_ex_impl* input[],
                                    const struct render_node_ex_impl* output[])
{
}

static struct render_node_ex_impl* dummy_root_node_impl_create()
{
        struct dummy_root_node_impl* self = alloc_obj(self);

        self->_parent.ops.f_compute = dummy_root_node_compute;
        return (struct render_node_ex_impl*) self;
}


#endif // RENDERNODEDUMMYIMPL_H_INCLUDED
