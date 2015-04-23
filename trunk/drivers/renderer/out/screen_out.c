/* screen_out.c: define render output through screen */
#include <system/log.h>
#include <math/math.h>
#include <renderer/out.h>
#include <system/allocator.h>
#include "screen_out.h"

/* TODO (davis#3#): <out> test this module */

#include <gtk/gtk.h>

struct gtk_out;
static struct gtk_out *create_gtk_out ( GtkWidget *widget,
                                        int x, int y, int width, int height,
                                        enum SCREEN_FORMAT_IDR format );
static void free_gtk_out ( struct gtk_out *out );
static void gtk_out_run ( struct gtk_out *out );
static void gtk_out_retrieve ( void *data, int size, struct gtk_out *out );
static gboolean display_callback ( struct _GtkWidget *widget,
                                   struct _cairo *cairo, gpointer data );


struct screen_out_ops {
        void *(*create) ( widget_ptr *region, int x, int y, int width, int height,
                          enum SCREEN_FORMAT_IDR format );
        void (*free) ( void *out );
        void (*retrieve) ( void *data, int size, void *out );
        void (*run) ( void *out );
};

static const struct screen_out_ops OutOps[] = {
        [OUT_GTK_IMPL].create = cast(OutOps->create)		create_gtk_out,
        [OUT_GTK_IMPL].free = cast(OutOps->free)		free_gtk_out,
        [OUT_GTK_IMPL].retrieve = cast(OutOps->retrieve)	gtk_out_retrieve,
        [OUT_GTK_IMPL].run = cast(OutOps->run)			gtk_out_run
};

void ro_create_screen ( widget_ptr *region, int x, int y, int width, int height,
                        enum SCREEN_FORMAT_IDR format, struct render_out *ro )
{
        ro->data = OutOps[ro->impl].create ( region, x, y, width, height, format );
}

void ro_screen_retrieve ( void *data, int size, struct render_out *ro )
{
        OutOps[ro->impl].retrieve ( data, size, ro->data );
}

void ro_screen_free ( struct render_out *ro )
{
        OutOps[ro->impl].free ( ro->data );
}

void ro_screen_run ( struct render_out *ro )
{
        OutOps[ro->impl].run ( ro->data );
}

/* gtk impl */
struct gtk_out {
        gboolean (*display_callback) ( struct _GtkWidget *widget,
                                       struct _cairo *cairo, gpointer data );
        int signal_handler;
        int signal_state;
        struct _GtkWidget *dst_widget;
        enum _cairo_format format;
        void *src_surface;
        int width, height;
        int x, y;
        int stride;
};

#define DRAW_SIGNAL_REMAIN	1
#define DRAW_SIGNAL_NONE	0

static struct gtk_out *create_gtk_out ( GtkWidget *widget,
                                        int x, int y, int width, int height,
                                        enum SCREEN_FORMAT_IDR format )
{
        struct gtk_out *out = alloc_fix ( sizeof *out, 1 );
        memset ( out, 0, sizeof *out );

        switch ( format ) {
        case OUT_SCREEN_INVALID: {
                out->format = CAIRO_FORMAT_INVALID;
                out->stride = 0;
                break;
        }
        case OUT_SCREEN_A1: {
                out->format = CAIRO_FORMAT_A1;
                out->stride = 1;
                break;
        }
        case OUT_SCREEN_A8: {
                out->format = CAIRO_FORMAT_A8;
                out->stride = 1;
                break;
        }
        case OUT_SCREEN_RGB16_565: {
                out->format = CAIRO_FORMAT_RGB16_565;
                out->stride = 2;
                break;
        }
        case OUT_SCREEN_RGB24: {
                out->format = CAIRO_FORMAT_RGB24;
                out->stride = 3;
                break;
        }
        case OUT_SCREEN_RGB30: {
                out->format = CAIRO_FORMAT_RGB30;
                out->stride = 4;
                break;
        }
        case OUT_SCREEN_ARGB32: {
                out->format = CAIRO_FORMAT_ARGB32;
                out->stride = 4;
                break;
        }
        default: {
                log_severe_err_dbg ( "color format is not supported by the api" );
                return nullptr;
        }
        }
        out->display_callback = display_callback;
        out->dst_widget = widget;
        out->signal_handler = g_signal_connect ( widget, "draw",
                              G_CALLBACK ( display_callback ), out );
        out->signal_state = DRAW_SIGNAL_NONE;
        return out;
}

static void free_gtk_out ( struct gtk_out *out )
{
        while ( out->signal_state == DRAW_SIGNAL_REMAIN ) {
                log_normal_dbg ( "Wait for DrawSignal being cleared" );
        }
        log_normal_dbg ( "signal cleared == 0, clear up" );
        g_signal_handler_disconnect ( out->dst_widget, out->signal_handler );
        free_fix ( out );
}

static void gtk_out_retrieve ( void *data, int size, struct gtk_out *out )
{
        memcpy ( &out->src_surface, data, size );
}

static void gtk_out_run ( struct gtk_out *out )
{
        gdk_threads_enter ();
        out->signal_state = DRAW_SIGNAL_REMAIN;
        gtk_widget_queue_draw ( out->dst_widget );
        gdk_threads_leave ();
}

static gboolean display_callback ( struct _GtkWidget *widget,
                                   struct _cairo *cairo, gpointer data )
{
        struct gtk_out *out = data;
        struct _cairo_surface *co_surface =
                cairo_image_surface_create_for_data ( out->src_surface,
                                out->format, out->width, out->height, out->stride );
        cairo_set_source_surface ( cairo, co_surface, out->x, out->y );
        cairo_paint ( cairo );
        cairo_surface_destroy ( co_surface );
        out->signal_state = DRAW_SIGNAL_NONE;
        return 0;
}
