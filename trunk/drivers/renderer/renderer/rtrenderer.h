#ifndef RT_RENDERER_H_INCLUDED
#define RT_RENDERER_H_INCLUDED


#include <algorithm.h>
#include <renderer/renderer.h>

struct alg_named_params;
struct proj_probe;
struct render_out;
struct rt_renderer;


/*
 * Functions' definition
 */
struct rt_renderer *create_rt_renderer ( void );
void free_rt_renderer ( struct rt_renderer *r );

void update_rt_renderer ( struct alg_llist *command, struct rt_renderer *r );
void rt_renderer_begin ( struct rt_renderer *r );
void rt_renderer_render ( struct proj_probe *probe, struct render_out *ro,
                          struct rt_renderer *r );
void rt_renderer_end ( struct rt_renderer *r );


#endif // RT_RENDERER_H_INCLUDED