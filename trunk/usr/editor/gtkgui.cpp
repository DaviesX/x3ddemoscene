#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"
#include "main_editor.hpp"
#include "entity_editor.hpp"
#include "renderable_editor.hpp"
#include "splash_screen.hpp"

using namespace x3d;
using namespace x3d::usr;


bool EditorGtkFrontend::init ( int argc, char **argv,
                               Editor *editor, KernelEnvironment *env )
{
        this->m_editor = editor;
        this->m_env = env;

        gtk_init ( &argc, &argv );
        gdk_threads_init ();

        /* determine the running mode from command line argument */
        int n = argc;
        char **params = argv;
        X_EDITOR_MODE mode = X_EDITOR_DEMO_MODE;
        int i;
        for ( i = 0; i < n; i ++ ) {
                char *buff = params[i];
                if ( !strcmp ( buff, "--edit-mode" ) ) {
                        mode = X_EDITOR_EDIT_MODE;
                        break;
                }
        }
        this->m_editor_mode = mode;

        if ( !this->splash_screen_load () ) {
                log_mild_err_dbg ( "couldn't load splash screen" );
                return false;
        }
        if ( !this->splash_screen_show ( true ) ) {
                log_mild_err_dbg ( "couldn't show splash screen" );
                return false;
        }
        return true;
}

bool EditorGtkFrontend::end_init ( Editor *editor, KernelEnvironment *env )
{
        this->m_editor = editor;
        this->m_env = env;
        return true;
}

bool EditorGtkFrontend::load ( Editor *editor, KernelEnvironment *env )
{
        this->m_editor = editor;
        this->m_env = env;

        if ( !this->main_editor_load () ) {
                log_severe_err_dbg ( "couldn't load gtk main editor" );
                return false;
        }
        if ( !this->render_config_load () ) {
                log_severe_err_dbg ( "couldn't load gtk renderer configurator" );
                return false;
        }
        if ( !entity_editor_load ( g_path_res.glade_path ) ) {
                log_severe_err_dbg ( "couldn't load gtk entity editor" );
                return false;
        }
        if ( !renderable_editor_load ( g_path_res.glade_path ) ) {
                log_severe_err_dbg ( "couldn't load gtk renderable editor" );
                return false;
        }
        return true;
}

void EditorGtkFrontend::loop ( Editor *editor, KernelEnvironment *env )
{
        this->m_editor = editor;
        this->m_env = env;

        if ( !this->main_editor_show ( true ) ) {
                log_severe_err_dbg ( "failed to show gtk main editor" );
                return ;
        }

        if ( !splash_screen_shut () ) {
                log_mild_err_dbg ( "couldn't shutdown the splash screen" );
                return ;
        }
        await_gtk_main ();
}

bool EditorGtkFrontend::free ( Editor *editor, KernelEnvironment *env )
{
        this->m_editor = editor;
        this->m_env = env;

        if ( !this->main_editor_shut () ) {
                log_severe_err_dbg ( "failed to shutdown gtk main editor" );
                return false;
        }
        return true;
}

GtkBuilder* builder_load ( string filename )
{
        GtkBuilder *builder = nullptr;
        if ( !(builder = gtk_builder_new () ) ) {
                log_severe_err_dbg ( "cannot create gtk-glade builder" );
                return nullptr;
        }
        GError *error = nullptr;
        if ( !(gtk_builder_add_from_file ( builder, filename.c_str (), &error )) ) {
                if ( error ) {
                        log_severe_err_dbg ( "builder fail to load: %s\n%s",
                                             filename.c_str (), error->message );
                        g_free ( error );
                } else {
                        log_severe_err_dbg ( "builder fail to load: %s, unknown error",
                                             filename.c_str () );
                }
                return nullptr;
        }
        return builder;
}

void builder_all_set ( GtkBuilder* builder )
{
        gtk_builder_connect_signals ( builder, nullptr );
        g_object_unref ( G_OBJECT ( builder ) );
}

/* @todo (davis#1#): implement file chooser */
string file_chooser_open ( string default_dir )
{
        return nullptr;
}

string file_chooser_save ( string default_dir )
{
        return nullptr;
}

void message_box_info ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

void message_box_warning ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

void message_box_error ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW(dialog), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gtk_dialog_run ( GTK_DIALOG(dialog) );
        gtk_widget_destroy ( dialog );
}

bool message_box_question ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_QUESTION, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW(dialog), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gint result = gtk_dialog_run ( GTK_DIALOG(dialog) );
        gtk_widget_destroy ( dialog );
        switch ( result ) {
        default:
        case GTK_RESPONSE_DELETE_EVENT:
        case GTK_RESPONSE_NO: {
                return false;
        }
        case GTK_RESPONSE_YES: {
                return true;
        }
        }
}

void widget_get_size ( GtkWidget* parent, GtkWidget* widget,
                       int* x, int* y, int* width, int* height )
{
        GtkAllocation *allocation = g_new0 ( GtkAllocation, 1 );
        gtk_widget_get_allocation ( widget, allocation );
        *width = allocation->width;
        *height = allocation->height;
        g_free ( allocation );
        gtk_widget_translate_coordinates ( widget, parent,
                                           0, 0, x, y );
}

static int              g_gtk_main_stack = 0;
static x3d::thr_trap    g_atomic_check = {0};
static x3d::thr_task*   g_gtk_main_task = nullptr;

static void* do_gtk_main ( void* data )
{
        {
                gdk_threads_enter ();
                gtk_main ();
                gdk_threads_leave ();
        }
        return nullptr;
}

#define push_gtk_main()         (g_gtk_main_stack ++)
#define pop_gtk_main()          (g_gtk_main_stack --)

void stop_gtk_main ( void )
{
        x3d::thr_trap_on_task ( &g_atomic_check );
        pop_gtk_main ();
        if ( g_gtk_main_stack == 0 ) {
                log_normal_dbg ( "no gtk main is running: %d", g_gtk_main_stack );
                {
                        gdk_threads_enter ();
                        gtk_main_quit ();
                        gdk_threads_leave ();
                        x3d::thr_sync_with_task ( g_gtk_main_task );
                        free_fix ( g_gtk_main_task );
                }
        } else {
                log_normal_dbg ( "found %d gtk main running already",
                                 g_gtk_main_stack );
        }
        x3d::thr_untrap_task ( &g_atomic_check );
}

void run_gtk_main ( void )
{
        x3d::thr_trap_on_task ( &g_atomic_check );
        if ( g_gtk_main_stack > 0 ) {
                log_normal_dbg ( "found %d gtk main running already",
                                 g_gtk_main_stack );
        } else {
                g_gtk_main_task =
                        x3d::thr_run_task ( do_gtk_main, nullptr, nullptr );
//                log_normal_dbg ( "start gtk main" );
        }
        push_gtk_main ();
        x3d::thr_untrap_task ( &g_atomic_check );
}

void await_gtk_main ( void )
{
        while ( g_gtk_main_stack != 0 );
        x3d::thr_trap_on_task ( &g_atomic_check );
        ;  /* do nothing */
        x3d::thr_untrap_task ( &g_atomic_check );
}

void* dbg_get_render_region ( void )
{
        return main_editor_get_region ();
}

void dbg_hand_image_to_display ( void *ptr, int w, int h )
{
        /*        g_comm_data.tmp_image = ptr;
                g_comm_data.tmp_image_w = w;
                g_comm_data.tmp_image_h = h;*/
//        main_editor_draw_tmp_image ();
}
