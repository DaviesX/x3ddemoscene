#ifndef RT_RENDERER_H_INCLUDED
#define RT_RENDERER_H_INCLUDED


#include <x3d/renderer.h>
#include <renderer/utility.h>

struct proj_probe;
struct render_out;
struct rt_renderer;


/*
 * Functions' definition
 */

/*
struct rt_renderer *create_rt_renderer ( enum RENDERER_IDR type );
void free_rt_renderer ( struct rt_renderer* r );

void rt_renderer_update ( struct render_bytecode* bytecode, struct rt_renderer* r );
void rt_renderer_render ( struct rt_renderer* r );
void rt_renderer_output ( struct rt_renderer* r );
 */

struct rt_radiance_node {
        struct render_node_ex_impl      _parent;
        struct util_image               target;
};

struct rt_renderable_loader_node {
        struct render_node_ex_impl _parent;

        struct rda_context* context;
};

struct rt_render_output_node {
        struct render_node_ex_impl _parent;

        struct perspective_probe* probe;
};


/*
 * functions' declarations
 */
void __public rt_renderer_system_init(struct symbol_set* symbol);

// radiance node
struct render_node_ex_impl* __callback rt_radiance_node_creator(struct render_node_ex* parent);
char* __implement rt_radiance_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree);
void __implement rt_radiance_node_compute(struct render_node_ex_impl* self,
                                          const struct render_node_ex_impl* input[],
                                          const struct render_node_ex_impl* output[]);
void* __implement rt_radiance_node_get_result(const struct render_node_ex_impl* self);
void __implement rt_radiance_node_free(struct render_node_ex_impl* self);

// renderable loader node
struct render_node_ex_impl* __callback rt_renderable_loader_node_creator(struct render_node_ex* parent);
char* __implement rt_renderable_loader_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree);
void __implement rt_renderable_loader_node_compute(struct render_node_ex_impl* self,
                                                   const struct render_node_ex_impl* input[],
                                                   const struct render_node_ex_impl* output[]);
void* __implement rt_renderable_loader_node_get_result(const struct render_node_ex_impl* self);
void __implement rt_renderable_loader_node_free(struct render_node_ex_impl* self);

// render output node
struct render_node_ex_impl* __callback rt_render_output_node_creator(struct render_node_ex* parent);
char* __implement rt_render_output_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree);
void __implement rt_render_output_node_compute(struct render_node_ex_impl* self,
                                               const struct render_node_ex_impl* input[],
                                               const struct render_node_ex_impl* output[]);
void* __implement rt_render_output_node_get_result(const struct render_node_ex_impl* self);
void __implement rt_render_output_node_free(struct render_node_ex_impl* self);


#endif // RT_RENDERER_H_INCLUDED
