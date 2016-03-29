#include <gtk/gtk.h>
#include <system/log.h>
#include <system/allocator.h>
#include <system/thread.h>
#include <x3d/display.h>


/*
 * <display> public
 */
void display_init(struct display* self, f_Display_Host_Image f_display_host, f_Display_Free f_free)
{
        self->f_display_host = f_display_host;
        self->f_free = f_free;
}

void display_free(struct display* self)
{
        self->f_free(self);
}

void display_host_image(struct display* self, struct host_image* image)
{
        self->f_display_host(self, image);
}


/*
 * <display_gtk_host> public
 */
struct display_gtk_host* display_gtk_host_create(GtkWidget* widget)
{
        struct display_gtk_host* self = alloc_obj(self);
        display_init(&self->_parent,
                     (f_Display_Host_Image) display_gtk_host_display_image,
                     (f_Display_Free) display_gtk_host_free);
        self->widget = widget;
        return self;
}

void display_gtk_host_free(struct display_gtk_host* self)
{
}


typedef gboolean (*f_GTK_Display_Callback) (struct _GtkWidget *widget, struct _cairo *cairo, gpointer data);
struct gtk_out {
        f_GTK_Display_Callback  display_callback;
        int                     signal_handler;
        int                     signal_state;
        struct _GtkWidget*      dst_widget;
        enum _cairo_format      format;
        void*                   src_surface;
        int                     width, height;
        int                     x, y;
        int                     stride;
};

#define DRAW_SIGNAL_REMAIN	1
#define DRAW_SIGNAL_NONE	0

static gboolean display_callback(struct _GtkWidget *widget, struct _cairo *cairo, gpointer data);


static struct gtk_out *gtk_out_create(GtkWidget *widget,
                                      int x, int y, int width, int height,
                                      enum ColorMode format, void* src_surface)
{
        struct gtk_out *out = alloc_fix(sizeof *out, 1);
        memset(out, 0, sizeof *out);

        switch(format) {
        case Color8Mode: {
                out->format = CAIRO_FORMAT_A1;
                out->stride = 1;
                break;
        }
        case Color16AMode: {
                out->format = CAIRO_FORMAT_RGB16_565;
                out->stride = 2;
                break;
        }
        case Color24Mode: {
                out->format = CAIRO_FORMAT_RGB24;
                out->stride = 3;
                break;
        }
        case Color32Mode: {
                out->format = CAIRO_FORMAT_ARGB32;
                out->stride = 4;
                break;
        }
        default: {
                log_severe_err_dbg("color format is not supported by the api");
                return nullptr;
        }
        }
        out->display_callback   = display_callback;
        out->dst_widget         = widget;
        out->signal_handler     = g_signal_connect(widget, "draw", G_CALLBACK(display_callback), out);
        out->signal_state       = DRAW_SIGNAL_NONE;
        out->src_surface        = src_surface;
        return out;
}

static void gtk_out_free(struct gtk_out *out)
{
        while(out->signal_state == DRAW_SIGNAL_REMAIN) {
                // log_normal_dbg("Wait for DrawSignal being cleared");
                thread_task_idle(1);
        }
        // log_normal_dbg("signal cleared == 0, clear up");
        g_signal_handler_disconnect(out->dst_widget, out->signal_handler);
        free_fix(out);
}

static void gtk_out_run(struct gtk_out *out)
{
//        gdk_threads_enter ();
        out->signal_state = DRAW_SIGNAL_REMAIN;
        gtk_widget_queue_draw(out->dst_widget);
//        gdk_threads_leave ();
}

static gboolean display_callback(struct _GtkWidget *widget,
                                 struct _cairo *cairo, gpointer data)
{
        struct gtk_out *out = data;
        struct _cairo_surface *co_surface =
                cairo_image_surface_create_for_data(out->src_surface,
                                out->format, out->width, out->height, out->stride);
        cairo_set_source_surface(cairo, co_surface, out->x, out->y);
        cairo_paint(cairo);
        cairo_surface_destroy(co_surface);
        out->signal_state = DRAW_SIGNAL_NONE;
        return 0;
}

void display_gtk_host_display_image(struct display_gtk_host* self, struct host_image* image)
{
        int w, h, stride, pix_size;
        enum ColorMode cmode;
        void* data = hostimg_export_level0(image, &cmode, &w, &h, &stride, &pix_size);
        struct gtk_out* out = gtk_out_create(self->widget, 0, 0, w, h, cmode, data);
        gtk_out_run(out);
        gtk_out_free(out);
}

/*
 * <display_image_file> public
 */
struct display_image_file* display_image_file_create(FILE* file)
{
        struct display_image_file* self = alloc_obj(self);
        display_init(&self->_parent,
                     (f_Display_Host_Image) display_image_file_display,
                     (f_Display_Free) display_image_file_free);
        self->file = file;
        return self;
}

void display_image_file_free(struct display_image_file* self)
{
        fclose(self->file);
}

void display_image_file_display(struct display_image_file* self, struct host_image* image)
{
}
