#include <gtk/gtk.h>
#include <system/log.h>
#include <system/allocator.h>
#include <system/thread.h>
#include <container/paramset.h>
#include <x3d/debug.h>
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
        struct _GtkWidget*      dst_widget;
        enum _cairo_format      format;
        void*                   src_surface;
        int                     width, height;
        int                     x, y;
        int                     stride;
};

static struct gtk_out *__gtk_out_create(GtkWidget *widget,
                                        int x, int y, int width, int height,
                                        enum ColorMode format, void* src_surface);
static void __gtk_out_free(struct gtk_out* out);
static void __gtk_out_run(struct gtk_out* out);


static gboolean __display_callback(GtkWidget* widget, cairo_t* cairo, gpointer data)
{
        struct gtk_out* out = data;
        cairo_surface_t* co_surface = cairo_image_surface_create_for_data(
                                out->src_surface, out->format, out->width, out->height, out->stride);
        cairo_set_source_surface(cairo, co_surface, out->x, out->y);
        cairo_paint(cairo);
        cairo_surface_destroy(co_surface);

        __gtk_out_free(out);
        return false;
}

static gboolean __idle_callback(gpointer data)
{
        struct gtk_out* out = (struct gtk_out*) data;
        gtk_widget_queue_draw(out->dst_widget);
        return false;
}

static struct gtk_out *__gtk_out_create(GtkWidget *widget,
                                        int x, int y, int width, int height,
                                        enum ColorMode format, void* src_surface)
{
        struct gtk_out *out = alloc_fix(sizeof *out, 1);
        memset(out, 0, sizeof *out);

        switch(format) {
        case Color8Mode:
                out->format = CAIRO_FORMAT_A1;
                break;
        case Color16AMode:
                out->format = CAIRO_FORMAT_RGB16_565;
                break;
        case Color24Mode:
                out->format = CAIRO_FORMAT_RGB24;
                break;
        case Color32Mode:
                out->format = CAIRO_FORMAT_ARGB32;
                break;
        default:
                log_severe_err_dbg("color format is not supported by the api");
                return nullptr;
        }
        out->width              = width;
        out->height             = height;
        out->stride             = cairo_format_stride_for_width(out->format, out->width);
        out->display_callback   = __display_callback;
        out->dst_widget         = widget;
        out->signal_handler     = g_signal_connect(widget, "draw", G_CALLBACK(__display_callback), out);
        out->src_surface        = src_surface;
        return out;
}

static void __gtk_out_free(struct gtk_out* out)
{
        g_signal_handler_disconnect(out->dst_widget, out->signal_handler);
        free_fix(out);
}

static void __gtk_out_run(struct gtk_out* out)
{
        g_idle_add(__idle_callback, out);
}

void display_gtk_host_display_image(struct display_gtk_host* self, struct host_image* image)
{
        int w, h, stride, pix_size;
        enum ColorMode cmode;
        void* data = hostimg_export_level0(image, &cmode, &w, &h, &stride, &pix_size);
        struct gtk_out* out = __gtk_out_create(self->widget, 0, 0, w, h, cmode, data);
        __gtk_out_run(out);
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


/*
 * <display_gtk_host> test cases
 */
void display_gtk_host_test_init(struct alg_var_set* envir) {};
void display_gtk_host_test_free(struct alg_var_set* envir) {};
enum DebugPosition* display_gtk_host_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
void display_gtk_host_test(struct alg_var_set* envir)
{
        int argc = *(int*) alg_var_set_use(envir, "argc");
        char** argv = *(char***) alg_var_set_use(envir, "argv");

        const int c_Width = 800, c_Height = 600;

        gtk_init(&argc, &argv);
        // create a gtk window
        GtkWidget *window;
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget* boxcont = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_container_add(GTK_CONTAINER(window), boxcont);

        GtkWidget* area = gtk_drawing_area_new();
        gtk_widget_set_size_request(area, c_Width, c_Height);
        gtk_box_set_center_widget(GTK_BOX(boxcont), area);

        gtk_widget_show_all(window);
        g_signal_connect(window, "destroy", gtk_main_quit, nullptr);
        // create a host image
        struct host_image image;
        hostimg_init(&image, 1, Color24Mode, c_Width, c_Height);
        hostimg_alloc(&image, 0);
        int i, j;
        for (j = 0; j < c_Height; j ++) {
                for (i = 0; i < c_Width; i ++) {
                        uint8_t* pix = hostimg_read(&image, 0, i, j);
                        pix[0] = 0;
                        pix[1] = 0;
                        pix[2] = 0;
                }
        }
        // create gtk host display
        struct display_gtk_host* gtkhost = display_gtk_host_create(area);
        display_gtk_host_display_image(gtkhost, &image);

        gtk_main();

        display_gtk_host_free(gtkhost);
        hostimg_free(&image);
}
