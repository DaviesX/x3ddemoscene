#ifndef IX3DRENDERING_H_INCLUDED
#define IX3DRENDERING_H_INCLUDED


/*
 * Functions' declaration
 */
renderer_handle_t create_renderer_x3d ( enum RENDERER_IDR idr );
renderer_handle_t load_renderer_from_file_x3d ( char *filename );
renderer_handle_t build_renderer_x3d ( void );
void free_renderer_x3d ( void );

void begin_edit_renderer_x3d ( void );
void end_edit_renderer_x3d ( void );
void set_screen_output_renderer_x3d ( enum OUTPUT_API_IDR idr, GtkWidget *widget,
				      int x, int y, int width, int height );
void set_image_output_renderer_x3d ( enum OUTPUT_API_IDR idr, enum IMAGE_FORMAT_IDR format, char *filename,
				     int x, int y, int width, int height );
void set_data_output_renderer_x3d ( enum OUTPUT_API_IDR idr, char *filename );
void set_output_crop_renderer_x3d ( struct irectangle2d *rect );
void set_color_mode_renderer_x3d ( enum SURFACE_IDR mode );
void set_background_color_renderer_x3d ( int is_to_fill, float r, float g, float b );
void set_proj_probe_renderer_x3d ( struct camera *cam_probe, int xres, int yres );
void set_distri_probe_renderer_x3d ( void );	/* Not implemented yet */
void set_polygon_mode_renderer_x3d ( enum RASTER_MODE_IDR idr );
void update_renderer_x3d ( void );
void stop_renderer_x3d ( void );

void begin_renderer_x3d ( void );
void draw_scene_renderer_x3d ( void );
void end_renderer_x3d ( void );


#endif // IX3DRENDERING_H_INCLUDED
