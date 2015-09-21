#ifndef PT_RENDERER_H_INCLUDED
#define PT_RENDERER_H_INCLUDED


#include <x3d/renderer.h>
#include <x3d/rendernoderadiance.h>
#include <x3d/rendernoderdaloader.h>
#include <x3d/rendernodeoutput.h>
#include <renderer/utility.h>
#include "raytracepipeline.h"

/*
 * structures
 */
struct pt_radiance_node {
        struct render_node_ex_impl      _parent;
        struct render_radiance          _parent2;
        struct pathtrace_pipeline*      pipeline;

        struct box3d*                   simplex;
        enum UtilAccessorType           acc_type;
        struct util_access*             acc_stt;
        struct util_stream              stream[10];
        int                             n_streams;
        struct util_aos                 aos_geo;
        struct util_aos                 aos_media;

        struct util_image               target;
};

struct pt_renderable_loader_node {
        struct render_node_ex_impl      _parent;
        struct render_rdaloader         _parent2;

        struct rda_context*             context;
};

struct pt_render_output_node {
        struct render_node_ex_impl      _parent;
        struct render_output            _parent2;

        struct perspective_probe*       probe;
};


/*
 * functions' declarations
 */
void __public           pt_renderer_system_init(struct symbol_set* symbol);

// radiance node
struct render_node_ex_impl* __callback  pt_radiance_node_creator(struct render_node_ex* parent);
bool __implement        pt_radiance_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree);
void __implement        pt_radiance_node_compute(struct render_node_ex_impl* self,
                                                 const struct render_node_ex_impl* input[],
                                                 const struct render_node_ex_impl* output[]);
void* __implement       pt_radiance_node_get_result(const struct render_node_ex_impl* self);
void __implement        pt_radiance_node_free(struct render_node_ex_impl* self);

// renderable loader node
struct render_node_ex_impl* __callback  pt_renderable_loader_node_creator(struct render_node_ex* parent);
bool __implement        pt_renderable_loader_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree);
void __implement        pt_renderable_loader_node_compute(struct render_node_ex_impl* self,
                                                          const struct render_node_ex_impl* input[],
                                                          const struct render_node_ex_impl* output[]);
void* __implement       pt_renderable_loader_node_get_result(const struct render_node_ex_impl* self);
void __implement        pt_renderable_loader_node_free(struct render_node_ex_impl* self);

// render output node
struct render_node_ex_impl* __callback  pt_render_output_node_creator(struct render_node_ex* parent);
bool __implement        pt_render_output_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree);
void __implement        pt_render_output_node_compute(struct render_node_ex_impl* self,
                                                      const struct render_node_ex_impl* input[],
                                                      const struct render_node_ex_impl* output[]);
void* __implement       pt_render_output_node_get_result(const struct render_node_ex_impl* self);
void __implement        pt_render_output_node_free(struct render_node_ex_impl* self);


#endif // PT_RENDERER_H_INCLUDED
