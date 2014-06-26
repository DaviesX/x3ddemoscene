#ifndef X3DRENDERING_H_INCLUDED
#define X3DRENDERING_H_INCLUDED


#include <x3d/renderer.h>


struct lcrenderer;

/*
 * Functions' declaration
 */
void lcrenderer_init ( void );
struct lcrenderer* lcrenderer_create ( enum RENDERER_IDR method );
void lcrenderer_free ( struct lcrenderer *rend );
void lcrenderer_update ( struct render_bytecode *bytecode, struct lcrenderer *rend );
void lcrenderer_render ( struct lcrenderer *rend );
void lcrenderer_output ( struct lcrenderer *rend );


#endif // X3DRENDERING_H_INCLUDED
