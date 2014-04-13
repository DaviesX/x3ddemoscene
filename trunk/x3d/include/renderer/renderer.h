#ifndef X3DRENDERING_H_INCLUDED
#define X3DRENDERING_H_INCLUDED


#include <x3d/renderer.h>
#include <renderer/geocache.h>


struct geocache;
struct probe;
struct render_out;
struct irenderer;

/*
 * Functions' declaration
 */
struct irenderer *icreate_renderer ( enum RENDERER_IDR method );
void ifree_renderer ( struct irenderer *r );
void iupdate_renderer ( struct alg_llist *command, struct irenderer *r );
void irenderer_begin ( struct irenderer *r );
void irenderer_render ( struct probe *probe, struct render_out *ro, struct irenderer *r );
void irenderer_end ( struct irenderer *r );
struct geocache *irenderer_export_geocache ( struct irenderer *r );
struct probe *irenderer_export_probe ( struct irenderer *r );


#endif // X3DRENDERING_H_INCLUDED
