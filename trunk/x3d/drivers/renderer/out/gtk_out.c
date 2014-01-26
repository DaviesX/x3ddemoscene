#include <logout.h>
#include "gtk_out.h"

/* TODO (davis#1#): Fix dependency of other modules from <out> module */

#define DRAW_SIGNAL_REMAIN	1
#define DRAW_SIGNAL_NONE	0

gboolean display_callback ( struct _GtkWidget *widget, struct _cairo *cairo, gpointer data );

struct cairo_surface *init_cairo_surface ( GtkWidget *widget,
		enum SURFACE_IDR img_format,
		struct irectangle2d *rect )
{
	struct cairo_surface *cs = alloc_fix ( sizeof ( *cs ), 1 );
	memset ( cs, 0, sizeof ( *cs ) );
//	int ps;
	switch ( img_format ) {
	case SF_IDR_I8: {
		cs->format = CAIRO_FORMAT_A8;
//		ps = 1;
		break;
	}
	case SF_IDR_IR5G6B5: {
		cs->format = CAIRO_FORMAT_RGB16_565;
//		ps = 2;
		break;
	}
	case SF_IDR_IR8G8B8: {
		cs->format = CAIRO_FORMAT_RGB24;
//		ps = 3;
		break;
	}
	case SF_IDR_IA8R8G8B8: {
		cs->format = CAIRO_FORMAT_ARGB32;
//		ps = 4;
		break;
	}
	default: {
		log_severe_err_dbg ( "color format is not supported by the api" );
		return nullptr;
	}
	}
	cs->display_callback = display_callback;
	cs->dst_widget = widget;
	cs->signal_handler = g_signal_connect ( widget, "draw", G_CALLBACK ( display_callback ), cs );
	cs->signal_state = DRAW_SIGNAL_NONE;
	return cs;
}

void free_cairo_surface ( struct cairo_surface *cs )
{
	while ( cs->signal_state == DRAW_SIGNAL_REMAIN ) {
		log_normal_dbg ( "Wait for DrawSignal being cleared" );
	}
	log_normal_dbg ( "isSentDrawSignal == 0, clear up" );
	g_signal_handler_disconnect ( cs->dst_widget, cs->signal_handler );
	free_fix ( cs );
}

void display_cairo_surface ( struct cairo_surface *cs )
{
	gdk_threads_enter ();
	gtk_widget_queue_draw ( cs->dst_widget );
	gdk_threads_leave ();
}

gboolean display_callback ( struct _GtkWidget *widget, struct _cairo *cairo, gpointer data )
{
	struct cairo_surface *cs = data;
	struct _cairo_surface *co_surface =
		cairo_image_surface_create_for_data ( cs->src_surface,
				cs->format, cs->w, cs->h, cs->p );
	cairo_set_source_surface ( cairo, co_surface, cs->x, cs->y );
	cairo_paint ( cairo );
	cairo_surface_destroy ( co_surface );
	cs->signal_state = DRAW_SIGNAL_NONE;
	return 0;
}
