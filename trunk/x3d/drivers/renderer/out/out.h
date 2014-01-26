#ifndef OUT_H_INCLUDED
#define OUT_H_INCLUDED


#include <gtk/gtk.h>
#include "../renderer/surface.h"


struct irectangle2d;

enum OUTPUT_API_IDR {
	OAPI_IDR_GTK,
	OAPI_IDR_OPENGL,
	OAPI_IDR_BUILTIN
};

enum IMAGE_FORMAT_IDR {
	IMGF_IDR_RAW,
	IMGF_IDR_BMP,
	IMGF_IDR_PNG,
	IMGF_IDR_OPENEXR,
	IMGF_IDR_TIFF,
	IMGF_IDR_JPEG
};

struct frame_screen_output {
	struct fso_operations *ops;
	enum OUTPUT_API_IDR oapi_idr;
	void *api_internal;
};

struct frame_image_output {
	struct fio_operations *ops;
	enum OUTPUT_API_IDR oapi_idr;
	void *api_internal;
};

void init_frame_screen_output ( enum OUTPUT_API_IDR api_idr, enum SURFACE_IDR format,
				struct irectangle2d *rect, GtkWidget *widget,
				struct frame_screen_output *fso );
void free_frame_screen_output ( struct frame_screen_output *fso );
void exe_frame_screen_output ( struct frame_screen_output *fso );

void init_frame_image_output ( enum OUTPUT_API_IDR api_idr, enum SURFACE_IDR format,
			       struct irectangle2d *rect,
			       char *filename,
			       struct frame_image_output *fio );
void free_frame_image_output ( struct frame_image_output *fio );


#endif // OUT_H_INCLUDED
