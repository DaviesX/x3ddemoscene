/* main_editor.cpp: implementation of the main editor control */
#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"
#include "main_editor.hpp"

using namespace x3d;
using namespace x3d::usr;


struct main_editor {
        EditorGtkFrontend::X_EDITOR_MODE           editor_mode;
        GtkBuilder*             builder;
        GtkWidget*              window;
        GtkWidget*              draw_region;
        GdkPixbuf*              logo_pix_buf;
        struct project_mgr*     proj_mgr;
        struct entity_mgr*      ent_mgr;
        struct entity_prop*     ent_prop;
        struct assist_dialog*   assist_diag;

        gulong                  logo_draw_signal;
        bool                    is_logo_connected;
};

static const string cOfficialEditor = "gtk-official-editor";
static const string cMainEditor = "gtk-main-editor";
static const string cRenderRegion = "gtk-main-editor-render-region";

/*
static struct main_editor g_main_edit;
*/

static void idle_switch_callback ( bool is_idle, EditorActiveX* ax, void *info );
static void window_reize_callback ( bool is_fullscreen, int width, int height,
                                    EditorActiveX* ax, void *info );
static void render_config_error_callback ( string message, RenderConfigActiveX *conf, void *data );

extern "C" gboolean display_logo_callback ( GtkWidget *draw_region,
                cairo_t *cairo, gpointer data );

extern "C" gboolean display_logo_callback ( GtkWidget *draw_region,
                cairo_t *cairo, gpointer data )
{
        struct main_editor* edit = (struct main_editor*) data;
        gdk_cairo_set_source_pixbuf ( cairo,
                                      edit->logo_pix_buf,
                                      0, 0 );
        cairo_paint ( cairo );
        cairo_fill ( cairo );
        return 0;
}

static void idle_switch_callback ( bool is_idle, EditorActiveX* ax, void *info )
{
        struct main_editor* edit = (struct main_editor*) info;
        GtkWidget *draw_area = (GtkWidget*) ((RenderRegionActiveX*) ax)->get_handle ();
        if ( is_idle ) {
                if ( !edit->is_logo_connected ) {
                        edit->logo_draw_signal =
                                g_signal_connect ( draw_area, "draw",
                                                   G_CALLBACK(display_logo_callback), edit );
                        gtk_widget_queue_draw ( edit->draw_region );
                        edit->is_logo_connected = true;
                }
        } else {
                if ( edit->is_logo_connected ) {
                        g_signal_handler_disconnect ( draw_area,
                                                      edit->logo_draw_signal );
                        GdkColor color;
                        color.red   = 0X0;
                        color.blue  = 0X0;
                        color.green = 0X0;
                        gtk_widget_modify_bg ( edit->draw_region, GTK_STATE_NORMAL, &color );
                        edit->is_logo_connected = false;
                }
        }
}

static void window_reize_callback ( bool is_fullscreen, int width, int height,
                                    EditorActiveX* ax, void *info )
{
}

extern "C" gboolean destroy_callback ( GtkWidget *widget, gpointer data )
{
        KernelEnvironment* env = (KernelEnvironment*) data;
        Editor* editor = (Editor*) env->use ( cOfficialEditor );
        editor->close ();
        stop_gtk_main ();
        return 0;
}

bool EditorGtkFrontend::main_editor_load ( void )
{
        struct main_editor* edit =
                (struct main_editor*) alloc_fix ( sizeof *edit, 1 );
        zero_obj ( edit );

        KernelEnvironment* env = this->m_env;
        env->declare ( cMainEditor, edit );
        env->declare ( cOfficialEditor, this->m_editor );

        /* set current control window */
        string file = this->m_glade_dir;
        GtkBuilder *builder = nullptr;

        if ( this->m_editor_mode == X_EDITOR_DEMO_MODE ) {
demo_mode:
                this->m_editor_mode = X_EDITOR_DEMO_MODE;

                file += this->m_demo_player;

                GtkBuilder *builder = nullptr;
                if ( !(builder = builder_load ( file )) )
                        return false;

                edit->window =
                        (GtkWidget *) gtk_builder_get_object ( builder, "demo-window" );
                if ( !edit->window ) {
                        log_severe_err_dbg ( "cannot retrieve demo window" );
                        return false;
                }
                edit->draw_region =
                        (GtkWidget *) gtk_builder_get_object ( builder, "demo-render-region" );
                if ( !edit->draw_region ) {
                        log_severe_err_dbg ( "cannot retrieve demo render region widget" );
                        return false;
                }
        } else if ( this->m_editor_mode == X_EDITOR_EDIT_MODE ) {
                file += this->m_main_editor;

                if ( !(builder = builder_load ( file )) )
                        return false;

                edit->window =
                        (GtkWidget *) gtk_builder_get_object ( builder, "main-editor-window" );
                if ( !edit->window ) {
                        log_severe_err_dbg ( "cannot retrieve main-editor window" );
                        return false;
                }
                edit->draw_region =
                        (GtkWidget *) gtk_builder_get_object ( builder, "DA-RenderRegion" );
                if ( !edit->draw_region ) {
                        log_severe_err_dbg ( "cannot retrieve draw-area widget" );
                        return false;
                }
        } else {
                log_mild_err_dbg ( "mode is not specified corrected. selected mode: %d. running into demo mode...", this->m_editor_mode );
                goto demo_mode;
        }
        // DON'T trash it yet.
        /* done using glade builder */
        // builder_all_set ( builder );
        edit->builder = builder;
        gtk_builder_connect_signals ( edit->builder, nullptr );

        /* change background color to black */
        GdkColor color;
        color.red   = 0X0;
        color.blue  = 0X0;
        color.green = 0X0;
        gtk_widget_modify_bg ( edit->draw_region, GTK_STATE_NORMAL, &color );

        /* load in logo file */
        GError *error = nullptr;
        string logo_file = this->m_glade_dir + this->m_logo;
        edit->logo_pix_buf =
                gdk_pixbuf_new_from_file ( logo_file.c_str (), &error );
        if ( error != nullptr ) {
                log_severe_err_dbg ( "couldn't load logo file: %s", logo_file.c_str () );
                log_severe_err_dbg ( "gtk internal message: %s", error->message );
                g_free ( error );
                return false;
        }

        /* create render region */
        int x, y;
        int width, height;
        widget_get_size ( edit->window, edit->draw_region,
                          &x, &y, &width, &height );
        RenderRegionActiveX* rrax = new RenderRegionActiveX ( cRenderRegion,
                        edit->draw_region,
                        x, y, width, height );
        rrax->bind_callback ( "notify_idle", (f_Generic) idle_switch_callback, edit );
        rrax->bind_callback ( "notify_resize", (f_Generic) window_reize_callback, edit );
        this->m_editor->add_activex ( rrax );
        return true;
}

bool EditorGtkFrontend::main_editor_show ( bool is_visible )
{
        gdk_threads_enter ();
        struct main_editor* edit =
                (struct main_editor*) this->m_env->use ( cMainEditor );

        /* set dispatch function which fetches signal
         * from core editor when gtk_main is idle
         * also, connect the window destroy signal */
        g_idle_add ( main_editor_dispatch, this->m_env );
        g_signal_connect ( G_OBJECT(edit->window), "destroy",
                           G_CALLBACK(destroy_callback), this->m_env );

        /* show window */
        if ( is_visible ) {
                gtk_widget_show_all ( edit->window );
                run_gtk_main ();
        } else {
                gtk_widget_hide ( edit->window );
        }
        gdk_threads_leave ();
        return true;
}

bool EditorGtkFrontend::main_editor_shut ( void )
{
        KernelEnvironment* env = this->m_env;

        struct main_editor* edit =
                (struct main_editor*) this->m_env->use ( cMainEditor );
        /* done using glade builder */
        builder_all_set ( edit->builder );

        env->undeclare ( cMainEditor );
        env->undeclare ( cOfficialEditor );
        return true;
}

void* EditorGtkFrontend::main_editor_get_builder ( void )
{
        struct main_editor* edit =
                (struct main_editor*) this->m_env->use ( cMainEditor );
        return edit->builder;
}

GtkWidget* main_editor_get_region ( void )
{
        return nullptr;
}

extern "C" gboolean main_editor_dispatch ( gpointer user_data )
{
        KernelEnvironment* env = (KernelEnvironment*) user_data;
        Editor* editor = (Editor*) env->use ( cOfficialEditor );
        editor->dispatch_signal ();
        return true;
}
