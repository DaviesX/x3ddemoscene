#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <logout.h>
#include <algorithm.h>
#include <x3d/editor.h>
#include <editor/editor.h>
#include "gui.h"
#include "main_editor.h"
#include "entity_editor.h"
#include "renderable_editor.h"
#include "splash_screen.h"


struct common_data g_comm_data;


bool editor_init ( int *argc, char ***argv )
{
        memset ( &g_comm_data, 0, sizeof g_comm_data );
        gtk_init ( argc, argv );

        /* determine the running mode from command line argument */
        int n = *argc;
        char **params = *argv;
        enum X_EDITOR_MODE mode = X_EDITOR_DEMO_MODE;
        int i;
        for ( i = 0; i < n; i ++ ) {
                char *buff = params[i];
                if ( !strcmp ( buff, "--edit-mode" ) ) {
                        mode = X_EDITOR_EDIT_MODE;
                        break;
                }
        }
        g_comm_data.ed_mode = mode;

        if ( !splash_screen_show ( g_path_res.glade_path ) ) {
                log_mild_err_dbg ( "couldn't show splash screen" );
                return false;
        }
        return true;
}

bool editor_end_init ( void )
{
        if ( !splash_screen_shut () ) {
                log_mild_err_dbg ( "couldn't shutdown the splash screen" );
                return false;
        }
        return true;
}

bool editor_load ( void )
{
        if ( !main_editor_load ( g_path_res.glade_path ) ) {
                return false;
        }
        if ( !entity_editor_load ( g_path_res.glade_path ) ) {
                return false;
        }
        if ( !renderable_editor_load ( g_path_res.glade_path ) ) {
                return false;
        }
        return true;
}

void editor_loop ( void *info )
{
        gtk_main ();
}

bool editor_free ( void )
{
        return false;
}

GtkBuilder *builder_load ( char *filename )
{
        GtkBuilder *builder = nullptr;
        if ( !(builder = gtk_builder_new () ) ) {
                log_severe_err_dbg ( "cannot create gtk-glade builder" );
                return nullptr;
        }
        GError *error = nullptr;
        if ( !(gtk_builder_add_from_file ( builder, filename, &error )) ) {
                if ( error ) {
                        log_severe_err_dbg ( "builder fail to load: %s\n%s",
                                             filename, error->message );
                        g_free ( error );
                } else {
                        log_severe_err_dbg ( "builder fail to load: %s, unknown error",
                                             filename );
                }
                return nullptr;
        }
        return builder;
}

void builder_all_set ( GtkBuilder *builder )
{
        gtk_builder_connect_signals ( builder, nullptr );
        g_object_unref ( G_OBJECT ( builder ) );
}

/* @todo (davis#1#): implement file chooser */
char *file_chooser_open ( char *default_dir )
{
        return nullptr;
}

char *file_chooser_save ( char *default_dir )
{
        return nullptr;
}

void message_box_info ( char *title, char *message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
                                          "%s", message );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

void message_box_warning ( char *title, char *message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
                                          "%s", message );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

void message_box_error ( char *title, char *message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                          "%s", message );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

bool message_box_question ( char *title, char *message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_QUESTION, GTK_BUTTONS_CLOSE,
                                          "%s", message );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title );
        gint result = gtk_dialog_run ( GTK_DIALOG ( dialog ) );
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

void widget_get_size ( GtkWidget *parent, GtkWidget *widget,
                       int *x, int *y, int *width, int *height )
{
        GtkAllocation *allocation = g_new0 ( GtkAllocation, 1 );
        gtk_widget_get_allocation ( widget, allocation );
        *width = allocation->width;
        *height = allocation->height;
        g_free ( allocation );
        gtk_widget_translate_coordinates ( widget, parent,
                                           0, 0, x, y );
}

void *dbg_get_render_region ( void )
{
        return main_editor_get_region ();
}

void dbg_hand_image_to_display ( void *ptr, int w, int h )
{
        g_comm_data.tmp_image = ptr;
        g_comm_data.tmp_image_w = w;
        g_comm_data.tmp_image_h = h;
//        main_editor_draw_tmp_image ();
}
