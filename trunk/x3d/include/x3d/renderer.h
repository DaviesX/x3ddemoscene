#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include <math/math.h>
#include <staging.h>
#include <algorithm.h>


/* The following rendering method is expected to engage in the renderer */
enum RENDERER_IDR {
	RENDERER_IDR_RASTERIZER,
	RENDERER_IDR_PATH_TRACER,
	RENDERER_IDR_PHOTON_TRACER,
	RENDERER_IDR_PHOTON_MAP,
	RENDERER_IDR_RADIOSITY,
	RENDERER_IDR_RADIANCE_CACHE,
	RENDERER_IDR_PRT
};

enum RENDERER_THREAD_STATE_IDR {
	/* Renderer should not handle threading from these constants itself,
	 * this are the states telling which situation
	 * the renderer is being created with */
	RENDERER_THREAD_MUTUAL = 0X0,
	RENDERER_THREAD_SEPARATED = 0X1,

	/* These are what renderer expected to behave internally */
	RENDERER_THREAD_SINGLE = 0X2,
	RENDERER_THREAD_MULTIPLE = 0X4,
};

enum RENDER_SPEC_IDR {
	RENDER_SPEC_SW_BUILTIN,
	RENDER_SPEC_HW_OPENGL,
	RENDER_SPEC_HW_DIRECTX
};

enum DRAW_MODE_IDR {
	DRAW_MODE_WIREFRAME,
	DRAW_MODE_SOLID
};

enum LIGHTING_IDR {
	LIGHTING_IDR_DIRECT = 0X1,
	LIGHTING_IDR_INDIRECT = 0X2,
	LIGHTING_IDR_SHADOW = 0X4
};

enum RENDER_CONFIG {
	RENDER_CONFIG_SPEC,
	RENDER_CONFIG_DRAW_MODE,
	RENDER_CONFIG_CROP,
	RENDER_CONFIG_BLEND_LAYER,
	RENDER_CONFIG_THREAD,
	RENDER_CONFIG_LIGHTING,
	RENDER_CONFIG_ANTIALIAS,
	RENDER_CONFIG_FILTERING
};

struct rc_params {
	int n_params;
	const char *type;
};

static const struct rc_params RenderConfig[] = {
	[RENDER_CONFIG_SPEC].n_params = 1,
	[RENDER_CONFIG_SPEC].type = "d",
	[RENDER_CONFIG_DRAW_MODE].n_params = 1,
	[RENDER_CONFIG_DRAW_MODE].type = "d",
	[RENDER_CONFIG_CROP].n_params = 4,
	[RENDER_CONFIG_CROP].type = "ffff",
	[RENDER_CONFIG_BLEND_LAYER].n_params = 2,
	[RENDER_CONFIG_BLEND_LAYER].type = "df",
	[RENDER_CONFIG_THREAD].n_params = 1,
	[RENDER_CONFIG_THREAD].type = "d",
	[RENDER_CONFIG_LIGHTING].n_params = 2,
	[RENDER_CONFIG_LIGHTING].type = "dd",
	[RENDER_CONFIG_ANTIALIAS].n_params = 1,
	[RENDER_CONFIG_ANTIALIAS].type = "d",
	[RENDER_CONFIG_FILTERING].n_params = 2,
	[RENDER_CONFIG_FILTERING].type = "dd"
};

enum PASS_CULLING_IDR {
	PASS_UNCULL = 0X0,
	PASS_CUBIC_CULL = 0X1,
	PASS_FRUSTUM_CULL = 0X2,
	PASS_OCCLUSION_CULL = 0X4
};

enum RENDERABLE_IDR {
	RENDERABLE_IDR_GEOMETRY,
	RENDERABLE_IDR_VOLUME
};

typedef uint32_t renderer_handle_t;
typedef uint32_t pass_id_t;
struct renderer;
struct scene;
struct geometry;
struct geocache;
struct volume;
struct alg_list;
struct cull_shape;
struct probe;
struct render_out;


/*
 * structures
 */
struct pass_desc {
	pass_id_t id;
	enum PASS_CULLING_IDR cull_idr;
	struct cull_shape *cull_shape;
};

struct renderable {
	enum RENDERABLE_IDR idr;
	void *entity;
};

struct renderer_ops {
	struct renderer *(*create) ( enum RENDERER_IDR method );
	/* void (*build) ( struct stager *stg, res_record_t *rec, void *r ); */
	void (*free) ( struct renderer *r );
	void (*update) ( struct alg_named_params *params, struct renderer *r );
	void (*begin) ( struct renderer *r );
	void (*render) ( struct probe *probe, struct render_out *ro, struct renderer *r );
	void (*end) ( struct renderer *r );
	struct geocache *(*export_geocache) ( struct renderer *r );
};

struct geocache_ops {
	void (*get_pass_desc) ( struct alg_list *desc, struct geocache *gc );
	void (*add) ( struct geometry *geo, pass_id_t id, struct geocache *gc );
	void (*add_from_pass) ( pass_id_t src, pass_id_t dest, struct geocache *gc );
	void (*flush) ( pass_id_t id, struct geocache *gc );
};

struct geometry_ops {
};

struct probe_ops {
};

/*
 * render context's declaration
 */
bool init_renderer_context ( void );
struct renderer_context *export_renderer_context ( void );
void renderer_context_import_renderer ( struct renderer_ops *ops );
void renderer_context_import_geocache ( struct geocache_ops *ops );
void renderer_context_import_geometry ( struct geometry_ops *ops );
void renderer_context_import_probe ( struct probe_ops *ops );
renderer_handle_t renderer_context_add ( enum RENDERER_IDR render_method );
void renderer_context_remove ( renderer_handle_t rh );
struct renderer_context *renderer_context_find_first ( renderer_handle_t *rh );
struct renderer_context *renderer_context_find_next ( renderer_handle_t *rh );

void update_renderer_context ( void );

void renderer_context_draw_scene ( renderer_handle_t rh, struct scene *scene );
void renderer_context_draw_geobatch ( renderer_handle_t rh, struct geometry *geo, int n );
void renderer_context_render ( renderer_handle_t rh );

void renderer_context_render_config ( renderer_handle_t rh, enum RENDER_CONFIG config );
void renderer_context_set_value ( renderer_handle_t rh, ... );

/*
 * rendergeo's declaration
 */


#endif // RENDERER_H_INCLUDED
