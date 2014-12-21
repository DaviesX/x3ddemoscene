#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"

using namespace x3d;
using namespace x3d::usr;

static GtkWidget* image  = nullptr;
static GtkWidget* window = nullptr;

bool EditorGtkFrontend::splash_screen_load ( void )
{
        string filename = this->m_glade_dir + this->m_logo;

        window = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
        gtk_widget_set_size_request ( window, 800, 600 );
        gtk_window_set_decorated ( GTK_WINDOW(window), FALSE );
        gtk_window_set_position ( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS );
        gtk_window_set_resizable ( GTK_WINDOW(window), FALSE );
        if ( !(image = gtk_image_new_from_file ( filename.c_str () )) ) {
                log_severe_err_dbg ( "couldn't load logo file from %s", filename.c_str() );
                return false;
        }
        gtk_container_add ( GTK_CONTAINER(window), image);
        return true;
}

bool EditorGtkFrontend::splash_screen_show ( bool is_visible )
{
        gtk_widget_show_all ( window );
        run_gtk_main ();
        return true;
}


bool EditorGtkFrontend::splash_screen_shut ( void )
{
        gdk_threads_enter ();
        gtk_widget_destroy ( image );
        gtk_widget_destroy ( window );
        gdk_threads_leave ();
        stop_gtk_main ();
        return true;
}
