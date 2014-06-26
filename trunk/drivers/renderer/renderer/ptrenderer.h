#ifndef PT_RENDERER_H_INCLUDED
#define PT_RENDERER_H_INCLUDED


#include <x3d/renderer.h>

/*
 * Functions' declarations
 */
struct pt_renderer *create_pt_renderer ( enum RENDERER_IDR type );
void free_pt_renderer ( struct pt_renderer* r );

void pt_renderer_update ( struct render_bytecode* bytecode, struct pt_renderer* r );
void pt_renderer_render ( struct pt_renderer* r );
void pt_renderer_output ( struct pt_renderer* r );


#endif // PT_RENDERER_H_INCLUDED
