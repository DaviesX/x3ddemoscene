/* main_editor.c: implementation of the main editor control */
#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include <x3d/editor.h>
#include "gui.h"
#include "main_editor.h"


struct main_editor {
        GtkWidget *curr_window;
        GtkWidget *curr_draw_area;
        GtkWidget *edit_window;
        GtkWidget *pure_window;
        GtkWidget *pure_draw_area;
        GtkWidget *draw_area;
        GtkWidget *element_tree_view;
        GtkWidget *new_assist_dialog;

        uuid_t editor_id;
        struct editor *editor;
        bool draw_signal_busy;
        int logo_draw_signal;
        int tmp_draw_signal;
};

static struct main_editor g_main_edit;

static void display_logo_image ( GtkWidget *draw_area, bool to_display );
static void switch_to_edit ( void );
static void switch_to_pure ( void );


static void switch_to_edit ( void )
{
}

static void switch_to_pure ( void )
{
}

static gboolean display_logo_callback ( GtkWidget *draw_region,
                                        cairo_t *cairo, gpointer data )
{
        gdk_cairo_set_source_pixbuf ( cairo,
                                      g_comm_data.logo_pix_buf,
                                      0, 0 );
        cairo_paint ( cairo );
        cairo_fill ( cairo );
        return 0;
}

static void display_logo_image ( GtkWidget *draw_area, bool to_display )
{
        if ( to_display && !g_main_edit.draw_signal_busy ) {
                g_main_edit.logo_draw_signal =
                        g_signal_connect ( draw_area, "draw",
                                           G_CALLBACK ( display_logo_callback ), nullptr );
        } else {
                g_signal_handler_disconnect ( draw_area,
                                              g_main_edit.logo_draw_signal );
        }
}

/* temporary functions */
static gboolean display_tmp_image_callback ( GtkWidget *draw_region,
                cairo_t *cairo, gpointer data )
{
        int stride = cairo_format_stride_for_width (
                             CAIRO_FORMAT_ARGB32, g_comm_data.tmp_image_w);
        struct _cairo_surface *co_surface = cairo_image_surface_create_for_data (
                        g_comm_data.tmp_image, CAIRO_FORMAT_ARGB32,
                        g_comm_data.tmp_image_w, g_comm_data.tmp_image_h, stride );
        cairo_set_source_surface ( cairo, co_surface, 0.0, 0.0 );
        cairo_paint ( cairo );
        cairo_surface_destroy ( co_surface );
        return 0;
}

static void display_tmp_image ( GtkWidget *draw_area, bool to_display )
{
        if ( to_display ) {
                g_main_edit.tmp_draw_signal = g_signal_connect (
                                                      draw_area, "draw",
                                                      G_CALLBACK ( display_tmp_image_callback ), nullptr );
                g_main_edit.draw_signal_busy = true;
        } else {
                g_signal_handler_disconnect ( draw_area,
                                              g_main_edit.tmp_draw_signal );
                g_main_edit.draw_signal_busy = false;
        }
}

void main_editor_draw_tmp_image ( void )
{
        display_logo_image ( g_main_edit.curr_draw_area, false );
        display_tmp_image ( g_main_edit.curr_draw_area, true );
}

bool main_editor_load ( char *glade_path )
{
        memset ( &g_main_edit, 0, sizeof g_main_edit );

        char file[256];
        strcpy ( file, glade_path );
        strcat ( file, "main_editor_window.glade" );
        GtkBuilder *builder = builder_load ( file );
        if ( builder == nullptr ) {
                return false;
        }

        g_main_edit.edit_window =
                GTK_WIDGET(gtk_builder_get_object ( builder, "main-editor" ));
        if ( !g_main_edit.edit_window ) {
                log_severe_err_dbg ( "cannot retrieve main-editor window" );
                return false;
        }
        g_main_edit.draw_area =
                GTK_WIDGET(gtk_builder_get_object ( builder, "draw-area" ));
        if ( !g_main_edit.draw_area ) {
                log_severe_err_dbg ( "cannot retrieve draw-area widget" );
                return false;
        }
        g_main_edit.pure_window =
                GTK_WIDGET(gtk_builder_get_object ( builder, "pure-window" ));
        if ( !g_main_edit.pure_window ) {
                log_severe_err_dbg ( "cannot retrieve pure-window" );
                return false;
        }
        g_main_edit.pure_draw_area =
                GTK_WIDGET(gtk_builder_get_object ( builder, "pure-draw-area" ));
        if ( !g_main_edit.pure_draw_area ) {
                log_severe_err_dbg ( "cannot retrieve pure-draw-area widget" );
                return false;
        }
        g_main_edit.element_tree_view =
                GTK_WIDGET(gtk_builder_get_object ( builder, "element-tree-view" ));
        if ( !g_main_edit.element_tree_view ) {
                log_severe_err_dbg ( "cannot retrieve element-tree-view widget" );
                return false;
        }
        g_main_edit.new_assist_dialog =
                GTK_WIDGET(gtk_builder_get_object ( builder, "new-assist-dialog" ));
        if ( !g_main_edit.new_assist_dialog ) {
                log_severe_err_dbg ( "cannot retrieve new-assist-dialog" );
                return false;
        }
        /* set current control window */
        if ( g_comm_data.ed_mode == X_EDITOR_DEMO_MODE ) {
demo_mode:
                g_main_edit.curr_window = g_main_edit.edit_window;
                g_main_edit.curr_draw_area = g_main_edit.draw_area;
        } else if ( g_comm_data.ed_mode == X_EDITOR_EDIT_MODE ) {
                g_main_edit.curr_window = g_main_edit.pure_window;
                g_main_edit.curr_draw_area = g_main_edit.pure_draw_area;
        } else {
                log_mild_err_dbg ( "mode is not specified corrected. selected mode: %d. running into demo mode...", g_comm_data.ed_mode );
                goto demo_mode;
        }
        /* done using glade builder */
        builder_all_set ( builder );

        /* change background color to black */
        GdkColor color;
        color.red   = 0X0;
        color.blue  = 0X0;
        color.green = 0X0;
        gtk_widget_modify_bg ( g_main_edit.curr_draw_area, GTK_STATE_NORMAL, &color );

        /* show window */
        gtk_widget_show_all ( g_main_edit.curr_window );

        /* create editor and upload render region */
        g_main_edit.editor_id = editor_add ( "main-window-editor" );
        g_main_edit.editor = editor_get_byid ( g_main_edit.editor_id );
        int x, y;
        int width, height;
        widget_get_size ( g_main_edit.curr_window, g_main_edit.curr_draw_area,
                          &x, &y, &width, &height );
        struct activex_render_region *render_region = create_activex_render_region (
                                PLATFORM_GTK, g_main_edit.curr_draw_area, x, y, width, height );
        editor_add_activex ( "main-window-render-region", render_region, g_main_edit.editor );

        display_logo_image ( g_main_edit.curr_draw_area, true );
        return true;
}

void main_editor_set ( int state )
{
}

GtkWidget *main_editor_get_region ( void )
{
        return g_main_edit.curr_draw_area;
}
