#include "X3dRenderLibrary.h"



renderer_handle_t create_renderer_x3d ( enum RENDERER_IDR idr )
{
	struct renderer r;
	renderer_handle_t rh = create_renderer ( idr, &r );
	push_renderer_core_context ( &r );
	return rh;
}

renderer_handle_t load_renderer_from_file_x3d ( char *filename )
{
}

renderer_handle_t build_renderer_x3d ( void )
{
	renderer_handle_t rh;
	struct renderer *r = get_renderer_core_context ( &rh );
	struct resource_piler *rp = get_dynamic_res_core_context ();
	/* If the renderer is not being created, that means we have to
	 * add it after it is being constructed, this case happens when
	 * a renderer is directly load from a file */
	if ( r == nullptr ) {
		struct renderer tmp_r;
		build_renderer ( rh, rp, &tmp_r );
		push_renderer_core_context ( &tmp_r );
	} else {
		build_renderer ( rh, rp, r );
	}
}

void free_renderer_x3d ( void )
{
	renderer_handle_t rh;
	struct renderer *r = get_renderer_core_context ( &rh );
	assert ( r );
	free_renderer ( r );
	pop_renderer_core_context ( rh );
}

void begin_edit_renderer_x3d ( void )
{
}

void end_edit_renderer_x3d ( void )
{
}

void set_screen_output_renderer_x3d ( enum OUTPUT_API_IDR idr, GtkWidget *widget,
				      int x, int y, int width, int height )
{
}

void set_image_output_renderer_x3d ( enum OUTPUT_API_IDR idr, enum IMAGE_FORMAT_IDR format, char *filename,
				     int x, int y, int width, int height )
{
}

void set_data_output_renderer_x3d ( enum OUTPUT_API_IDR idr, char *filename )
{
}

void set_output_crop_renderer_x3d ( struct irectangle2d *rect )
{
}

void set_color_mode_renderer_x3d ( enum SURFACE_IDR mode )
{
}

void set_background_color_renderer_x3d ( int is_to_fill, float r, float g, float b )
{
}

void set_proj_probe_renderer_x3d ( struct camera *cam_probe, int xres, int yres )
{
}

void set_distri_probe_renderer_x3d ( void )
{
}

void set_polygon_mode_renderer_x3d ( enum RASTER_MODE_IDR idr )
{
}

void update_renderer_x3d ( void )
{
}

void stop_renderer_x3d ( void )
{
}

void begin_renderer_x3d ( void )
{
	renderer_handle_t rh;
	struct renderer *r = get_renderer_core_context ( &rh );
	assert ( r );
	begin_renderer ( r );
}

void draw_scene_renderer_x3d ( void )
{
}

void end_renderer_x3d ( void )
{
	renderer_handle_t rh;
	struct renderer *r = get_renderer_core_context ( &rh );
	assert ( r );
	end_renderer ( r );
}
