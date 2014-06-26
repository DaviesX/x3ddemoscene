#ifndef RT_RENDERER_H_INCLUDED
#define RT_RENDERER_H_INCLUDED


#include <algorithm.h>
#include <x3d/renderer.h>

struct proj_probe;
struct render_out;
struct rt_renderer;


/*
 * Functions' definition
 */
struct rt_renderer *create_rt_renderer ( enum RENDERER_IDR type );
void free_rt_renderer ( struct rt_renderer* r );

void rt_renderer_update ( struct render_bytecode* bytecode, struct rt_renderer* r );
void rt_renderer_render ( struct rt_renderer* r );
void rt_renderer_output ( struct rt_renderer* r );


#endif // RT_RENDERER_H_INCLUDED
