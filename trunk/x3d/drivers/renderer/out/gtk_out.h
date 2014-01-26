#ifndef GTK_OUT_H_INCLUDED
#define GTK_OUT_H_INCLUDED


#include <gtk/gtk.h>
#include "../renderer/surface.h"


struct cairo_surface {
	gboolean (*display_callback) ( struct _GtkWidget *widget,
				       struct _cairo *cairo, gpointer data );
	int signal_handler;
	int signal_state;
	struct _GtkWidget *dst_widget;
	enum _cairo_format format;
	void *src_surface;
	int w, h, p;
	int x, y;
};

struct irectangle2d;

struct cairo_surface *init_cairo_surface ( GtkWidget *widget,
		enum SURFACE_IDR img_format,
		struct irectangle2d *rect );
void free_cairo_surface ( struct cairo_surface *cs );
void display_cairo_surface ( struct cairo_surface *cs );


#endif // GTK_OUT_H_INCLUDED
