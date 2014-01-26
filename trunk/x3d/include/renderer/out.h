#ifndef OUT_H_INCLUDED
#define OUT_H_INCLUDED


enum OUT_IDR {
	OUT_IDR_SCREEN,
	OUT_IDR_STREAM,
	OUT_IDR_IMAGE,
	OUT_IDR_BINARY
};

enum OUT_IMPL_IDR {
	OUT_BUILTIN_IMPL,
	OUT_GTK_IMPL,
	OUT_QT_IMPL,
	OUT_OPENGL_IMPL
};

struct render_out {
	enum OUT_IDR idr;
	enum OUT_IMPL_IDR impl;
	void *data;
};


/*
 * functions' declaration
 */
void init_render_out ( enum OUT_IDR method, enum OUT_IMPL_IDR impl,
		       struct render_out *ro );
void free_render_out ( struct render_out *ro );
void render_out_run ( struct render_out *ro );
void render_out_retrieve ( void *data, int size, struct render_out *ro );

/* screen out */
typedef void* widget_ptr;

enum SCREEN_FORMAT_IDR {
	OUT_SCREEN_INVALID,
	OUT_SCREEN_ARGB32,
	OUT_SCREEN_RGB24,
	OUT_SCREEN_A8,
	OUT_SCREEN_A1,
	OUT_SCREEN_RGB16_565,
	OUT_SCREEN_RGB30
};

void ro_create_screen ( widget_ptr *region, int x, int y, int width, int height,
			enum SCREEN_FORMAT_IDR format, struct render_out *ro );

/* stream out */
void ro_create_stream ( struct render_out *ro );
void *ro_stream_export ( struct render_out *ro, int *count );

/* image out */
enum IMAGE_FORMAT_IDR {
	OUT_IMAGE_RAW,
	OUT_IMAGE_BMP,
	OUT_IMAGE_PNG,
	OUT_IMAGE_OPENEXR
};

void ro_create_image ( char *filename, enum IMAGE_FORMAT_IDR format,
		       struct render_out *ro );

/* binary out */
void ro_create_binary ( char *filename, struct render_out *ro );


#endif // OUT_H_INCLUDED
