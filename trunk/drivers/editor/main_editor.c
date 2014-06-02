/* main_editor.c: implementation of the main editor control */
#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include <x3d/editor.h>
#include "gui.h"
#include "main_editor.h"


struct main_editor {
        GtkWidget *window;
        GtkWidget *draw_region;
        struct project_mgr *proj_mgr;
        struct entity_mgr *ent_mgr;
        struct entity_prop *ent_prop;
        struct assist_dialog *assist_diag;

        gulong logo_draw_signal;

        struct editor *editor;
        uuid_t editor_id;
};

static struct main_editor g_main_edit;

static void idle_switch_callback ( bool is_idle, void *handle, void *info );
static void window_reize_callback ( int width, int height, bool is_fullscreen,
                                    void *handle, void *info );
static gboolean display_logo_callback ( GtkWidget *draw_region,
                                        cairo_t *cairo, gpointer data );
static gboolean main_editor_dispatch ( gpointer user_data );



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

static void idle_switch_callback ( bool is_idle, void *handle, void *info )
{
        GtkWidget *draw_area = handle;
        if ( is_idle ) {
                g_main_edit.logo_draw_signal =
                        g_signal_connect ( draw_area, "draw",
                                           G_CALLBACK ( display_logo_callback ), nullptr );
        } else {
                g_signal_handler_disconnect ( draw_area,
                                              g_main_edit.logo_draw_signal );
        }
}

static void window_reize_callback ( int width, int height, bool is_fullscreen,
                                    void *handle, void *info )
{
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

bool main_editor_load ( char *glade_path )
{
        memset ( &g_main_edit, 0, sizeof g_main_edit );

        /* set current control window */
        char file[256];
        GtkBuilder *builder = nullptr;

        if ( g_comm_data.ed_mode == X_EDITOR_DEMO_MODE ) {
demo_mode:
                strcpy ( file, glade_path );
                strcat ( file, "demo_player.glade" );

                GtkBuilder *builder = nullptr;
                if ( !(builder = builder_load ( file )) )
                        return false;

                g_main_edit.window =
                        (GtkWidget *) gtk_builder_get_object ( builder, "demo-window" );
                if ( !g_main_edit.window ) {
                        log_severe_err_dbg ( "cannot retrieve demo window" );
                        return false;
                }
                g_main_edit.draw_region =
                        (GtkWidget *) gtk_builder_get_object ( builder, "demo-render-region" );
                if ( !g_main_edit.draw_region ) {
                        log_severe_err_dbg ( "cannot retrieve demo render region widget" );
                        return false;
                }
        } else if ( g_comm_data.ed_mode == X_EDITOR_EDIT_MODE ) {
                strcpy ( file, glade_path );
                strcat ( file, "main-editor-window.glade" );


                if ( !(builder = builder_load ( file )) )
                        return false;

                g_main_edit.window =
                        (GtkWidget *) gtk_builder_get_object ( builder, "main-editor-window" );
                if ( !g_main_edit.window ) {
                        log_severe_err_dbg ( "cannot retrieve main-editor window" );
                        return false;
                }
                g_main_edit.draw_region =
                        (GtkWidget *) gtk_builder_get_object ( builder, "render-region" );
                if ( !g_main_edit.draw_region ) {
                        log_severe_err_dbg ( "cannot retrieve draw-area widget" );
                        return false;
                }
        } else {
                log_mild_err_dbg ( "mode is not specified corrected. selected mode: %d. running into demo mode...", g_comm_data.ed_mode );
                goto demo_mode;
        }
        /* done using glade builder */
        builder_all_set ( builder );

        /* set dispatch function which fetches signal
         * from core editor when gtk_main is idle */
        g_idle_add ( main_editor_dispatch, nullptr );

        /* change background color to black */
        GdkColor color;
        color.red   = 0X0;
        color.blue  = 0X0;
        color.green = 0X0;
        gtk_widget_modify_bg ( g_main_edit.draw_region, GTK_STATE_NORMAL, &color );

        /* show window */
        gtk_widget_show_all ( g_main_edit.window );

        /* create editor */
        g_main_edit.editor_id = editor_add ( "main-window-editor" );
        g_main_edit.editor = editor_get_byid ( g_main_edit.editor_id );
        /* create render region */
        int x, y;
        int width, height;
        widget_get_size ( g_main_edit.window, g_main_edit.draw_region,
                          &x, &y, &width, &height );
        struct ax_render_region *render_region = create_ax_render_region (
                                PLATFORM_GTK, g_main_edit.draw_region, x, y, width, height );
        ax_render_region_bind_signal ( "notify_idle", (f_Generic) idle_switch_callback,
                                       nullptr, render_region );
        editor_add_activex ( "main-window-render-region", render_region, g_main_edit.editor );
        return true;
}

GtkWidget *main_editor_get_region ( void )
{
        return g_main_edit.draw_region;
}

static gboolean main_editor_dispatch ( gpointer user_data )
{
        editor_dispatch_signal ( g_main_edit.editor );
        return true;
}
