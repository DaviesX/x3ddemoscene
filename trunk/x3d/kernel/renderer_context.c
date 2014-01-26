#include <x3d/renderer.h>
#include <renderer/renderer.h>
#include <renderer/geocache.h>


#define MAX_RENDER_CONTEXT		32

struct renderer_context {
	renderer_handle_t rh;
	struct renderer *rend;
	struct geocache *gc;
};

static struct renderer_context g_rend_con[MAX_RENDER_CONTEXT] = {0};
static struct alg_llist rend_llist = {0};

static struct renderer_ops g_rend_ops = {
	.create = create_renderer,
	.free = free_renderer,
	.update = update_renderer,
	.begin = renderer_begin,
	.render = renderer_render,
	.end = renderer_end,
	.export_geocache = renderer_export_geocache
};

static struct geocache_ops g_gc_ops = {
	.get_pass_desc = geocache_send_passes_desc,
	.add = geocache_add_geometry,
	.add_from_pass = geocache_add_from_pass,
	.flush = geocache_flush
};


bool init_renderer_context ( void )
{
	return true;
}

struct renderer_context *export_renderer_context ( void )
{
	return g_rend_con;
}

void renderer_context_import_renderer ( struct renderer_ops *ops )
{
}

void renderer_context_import_geocache ( struct geocache_ops *ops )
{
}

void renderer_context_import_geometry ( struct geometry_ops *ops )
{
}

void renderer_context_import_probe ( struct probe_ops *ops )
{
}

renderer_handle_t renderer_context_add ( enum RENDERER_IDR render_method,
		struct renderer_context *rend_con )
{
}

void renderer_context_remove ( renderer_handle_t rh )
{
}

struct renderer_context *renderer_context_find_first ( renderer_handle_t *rh )
{
}

struct renderer_context *renderer_context_find_next ( renderer_handle_t *rh )
{
}

void update_renderer_context ( void )
{
}

/* These drawing function will feed renderable data to the renderer's corresponding cache */
void renderer_context_draw_scene ( renderer_handle_t rh, struct scene *scene )
{
}

void renderer_context_draw_geobatch ( renderer_handle_t rh, struct geometry *geo, int n )
{
}

/* The render function of the local renderer will be invoked */
void renderer_context_render ( renderer_handle_t rh )
{
}

void renderer_context_render_config ( renderer_handle_t rh, enum RENDER_CONFIG config )
{
}

void renderer_context_set_value ( renderer_handle_t rh, ... )
{
}
