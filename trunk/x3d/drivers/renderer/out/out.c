#include "out.h"
#include "gl_out.h"
#include "gtk_out.h"
#include "image_out.h"

/* FIXME (davis#1#): reduce depency of other modules from <out> */



#define MAX_APIS		2

struct fso_operations {
	void *(*init) ( struct _GtkWidget *widget, enum SURFACE_IDR f, struct irectangle2d *r );
	void (*free) ( void *api_internal );
	void (*display) ( void *api_internal );
};

struct fio_operations {
	void (*generate) ( void *api_internal );
};

const struct fso_operations FSOOps[MAX_APIS] = {
	[OAPI_IDR_GTK].init = cast(FSOOps->init) init_cairo_surface,
	[OAPI_IDR_GTK].free = cast(FSOOps->free) free_cairo_surface,
	[OAPI_IDR_GTK].display = cast(FSOOps->display) display_cairo_surface
};

const struct fio_operations FIOOps[MAX_APIS] = {
	nullptr
};


void init_frame_screen_output ( enum OUTPUT_API_IDR api_idr, enum SURFACE_IDR format,
				struct irectangle2d *rect, GtkWidget *widget,
				struct frame_screen_output *fso )
{
	fso->oapi_idr = api_idr;
	fso->ops = &FSOOps[api_idr];
	fso->api_internal = fso->ops->init ( widget, format, rect );
}

void free_frame_screen_output ( struct frame_screen_output *fso )
{
	fso->ops->free ( fso->api_internal );
	memset ( fso, 0, sizeof ( *fso ) );
}

void exe_frame_screen_output ( struct frame_screen_output *fso )
{
	fso->ops->display ( fso->api_internal );
}

void init_frame_image_output ( enum OUTPUT_API_IDR api_idr, enum SURFACE_IDR format,
			       struct irectangle2d *rect,
			       char *filename,
			       struct frame_image_output *fio )
{
}

void free_frame_image_output ( struct frame_image_output *fio )
{
}
