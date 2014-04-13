#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <logout.h>
#include <algorithm.h>
#include <x3d/editor.h>
#include <editor/editor.h>
#include "gui.h"
#include "main_editor.h"
#include "object_editor.h"
#include "renderable_editor.h"


struct common_data g_comm_data;

bool load_editor ( int *argc, char ***argv )
{
        memset ( &g_comm_data, 0, sizeof g_comm_data );

        /* load in glade file */
        gtk_init ( argc, argv );
        GtkBuilder *builder = nullptr;
        if ( !(builder = gtk_builder_new () ) ) {
                log_severe_err_dbg ( "cannot create gtk-glade builder" );
                return false;
        }
        GError *error = nullptr;
        if ( !(gtk_builder_add_from_file ( builder,
                                           g_path_res.glade_file, &error )) ) {
                if ( error ) {
                        log_severe_err_dbg ( "builder fail to load: %s\n%s",
                                             g_path_res.glade_file,
                                             error->message );
                        g_free ( error );
                } else {
                        log_severe_err_dbg ( "builder fail to load: %s, unknown error",
                                             g_path_res.glade_file );
                }
                return false;
        }
        /* create logo */
        GdkPixbuf *pix_buf =
                gdk_pixbuf_new_from_file ( g_path_res.logo_file, &error );
        if ( !pix_buf ) {
                log_severe_err_dbg ( "fail to load logo image: %s\n%s",
                                     g_path_res.logo_file,
                                     error->message );
                g_free ( error );
                return false;
        }
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
        g_comm_data.builder = builder;
        g_comm_data.logo_pix_buf = pix_buf;
        if ( !main_editor_load () ) {
                return false;
        }
        if ( !object_editor_load () ) {
                return false;
        }
        if ( !renderable_editor_load () ) {
                return false;
        }
        gtk_builder_connect_signals ( builder, nullptr );
        g_object_unref ( G_OBJECT ( builder ) );
        return true;
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
        main_editor_draw_tmp_image ();
}

/** TEMP **/
enum RP_SCENE_ERR {
        RP_SCENE_NORMAL,
        RP_SCENE_LOADED,
        RP_SCENE_LOAD_FAILED,
        RP_SCENE_NONEXIST
};

/* Contains all widgets in the main rendering window */
struct main_window {
        GtkWidget *window;
        GtkWidget *win_box;
        GtkWidget *menu_bar;
        GtkWidget *draw_region;
        GtkWidget *status_bar;
        GtkWidget *aboout_dialog;
        GtkWidget *scene_file_chooser;
        GtkWidget *renderer_file_chooser;
        GtkWidget *raster_settings_dialog;
        GdkPixbuf *logo_pix_buf;
};



static struct main_window g_main_win;

gboolean display_logo_callback ( GtkWidget *draw_region, cairo_t *cairo, gpointer data );
gboolean quit_callback ( GtkWidget *window );
void about_menu_callback ( GtkWidget *dialog );
void open_scene_menu_callback ( GtkWidget *file_chooser );
void open_scene_file_callback ( GtkWidget *file_chooser );
void close_scene_menu_callback ( GtkWidget *window );
void copy_image_menu_callback ( GtkWidget *window );
void render_menu_callback ( GtkWidget *window );
void test_case_menu_callback ( GtkWidget *window );
void stop_render_menu_callback ( GtkWidget *window );
void display_scene_menu_callback ( GtkWidget *window );
void pop_msg_box ( char *content, GtkWidget *window, GtkMessageType type );
void update_current_info_bridge ( void );


bool load_main_window ( int *argc, char ***argv )
{
        gtk_init ( argc, argv );
        GtkBuilder *builder = nullptr;
        if ( !(builder = gtk_builder_new () ) ) {
                return false;
        }
        memset ( &g_main_win, 0, sizeof ( g_main_win ) );

        /* Load in glade file */
        GError *error = nullptr;

        if ( !(gtk_builder_add_from_file ( builder, g_path_res.glade_file, &error )) ) {
                if ( error ) {
                        puts ( "builder load fail: " );
                        puts ( error->message );
                        free ( error );
                } else {
                        puts ( "builder load fail, unknown error" );
                }
                return false;
        }

        /* Load in widgets */
        if ( !(g_main_win.window =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "RenderWindow" ) )) ) {
                return false;
        }
        if ( !(g_main_win.win_box =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "MainBox" ) )) ) {
                return false;
        }
        if ( !(g_main_win.menu_bar =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "menubar1" ) )) ) {
                return false;
        }
        if ( !(g_main_win.status_bar =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "statusbar1" ) )) ) {
                return false;
        }
        if ( !(g_main_win.draw_region =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "DrawRegion" ) )) ) {
                return false;
        }
        if ( !(g_main_win.aboout_dialog =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "X3dAboutDialog" ) )) ) {
                return false;
        }
        if ( !(g_main_win.scene_file_chooser =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "SceneFileChooser" ) )) ) {
                return false;
        }
        if ( !(g_main_win.renderer_file_chooser =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "SceneFileChooser" ) )) ) {
                return false;
        }
        if ( !(g_main_win.raster_settings_dialog =
                       GTK_WIDGET( gtk_builder_get_object ( builder, "RasterSettingsDialog" ) )) ) {
                return false;
        }

        gtk_builder_connect_signals ( builder, nullptr );
        g_object_unref ( G_OBJECT ( builder ) );
        gtk_widget_show_all ( g_main_win.window );

        GdkColor color;
        color.red = color.blue = color.green = 0X0;
        gtk_widget_modify_bg ( g_main_win.draw_region, GTK_STATE_NORMAL, &color );

        /* Display logo */
        g_main_win.logo_pix_buf =
                gdk_pixbuf_new_from_file ( g_path_res.logo_file, &error );

        if ( !g_main_win.logo_pix_buf ) {
                printf ( "Cannot load logo file: %s", g_path_res.logo_file );
                puts ( error->message );
                free ( error );
                return false;
        }

        g_signal_connect ( g_main_win.draw_region, "draw",
                           G_CALLBACK ( display_logo_callback ), nullptr );
        g_signal_connect ( g_main_win.window, "destroy",
                           G_CALLBACK ( quit_callback ), nullptr );
        free ( error );
        /* create named parameters set and update it with current info */
//        g_info_bridge = get_info_bridge ();
        update_current_info_bridge ();
        /* add main render region */
        /* draw region size and position */
        GtkAllocation *allocation = g_new0 ( GtkAllocation, 1 );
        gtk_widget_get_allocation ( g_main_win.draw_region, allocation );
        gint width = allocation->width;
        gint height = allocation->height;
        g_free ( allocation );
        gint x, y;
        gtk_widget_translate_coordinates ( g_main_win.draw_region,
                                           g_main_win.win_box, 0, 0, &x, &y );

        struct activex_render_region *render_region = create_activex_render_region (
                                PLATFORM_GTK, g_main_win.draw_region, x, y, width, height );
        uuid_t editor = editor_add ( "main-window-editor" );
        editor_add_activex ( "main-window-render-region", render_region,
                             editor_get_byid ( editor ) );
        return true;
}

gboolean quit_callback ( GtkWidget *window )
{
        gtk_main_quit ();
        return false;
}

void about_menu_callback ( GtkWidget *dialog )
{
        /* show about dialog */
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_hide ( dialog );
}

void open_scene_menu_callback ( GtkWidget *file_chooser )
{
        /* Set default path for scenes */
        gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER ( file_chooser ),
                                              g_path_res.raw_media_dir );
        gtk_dialog_run ( GTK_DIALOG ( file_chooser ) );
        gtk_widget_hide ( file_chooser );
}

/* open button of the scene file chooser is clicked */
void open_scene_file_callback ( GtkWidget *file_chooser )
{
        char *name = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( file_chooser ) );
        log_normal_dbg ( "opening scene: %s", name );
        /** FIXME: scene loading function **/
        /* enum RP_SCENE_ERR err = rp_load_scene ( name ); */
        uint32_t err = 0;
        switch ( err ) {
        case RP_SCENE_LOAD_FAILED: {
                pop_msg_box ( "Failed to load scene file", nullptr, GTK_MESSAGE_ERROR );
                break;
        }
        case RP_SCENE_NORMAL: {
                pop_msg_box ( "Scene has been loaded", nullptr, GTK_MESSAGE_INFO );
                break;
        }
        }
        free ( name );
}

void close_scene_menu_callback ( GtkWidget *window )
{
        /* enum RP_SCENE_ERR err = rp_unload_scene (); */
        uint32_t err = 0;
        switch ( err ) {
        case RP_SCENE_NONEXIST: {
                pop_msg_box ( "No scene to unload", nullptr, GTK_MESSAGE_WARNING );
                break;
        }
        case RP_SCENE_NORMAL: {
                pop_msg_box ( "Scene has been unloaded", nullptr, GTK_MESSAGE_INFO );
                break;
        }
        }
}

void copy_image_menu_callback ( GtkWidget *window )
{
}

void render_menu_callback ( GtkWidget *window )
{
        update_current_info_bridge ();
        /*	rp_update_renderer ();
        	rp_render ();*/
}

void display_scene_menu_callback ( GtkWidget *window )
{
        update_current_info_bridge ();
        /*	rp_update_renderer ();
        	rp_render ();*/
}

void test_case_menu_callback ( GtkWidget *window )
{
        update_current_info_bridge ();
        /*	rp_update_renderer ();
        	rp_test ();*/
}

void stop_render_menu_callback ( GtkWidget *window )
{
        update_current_info_bridge ();
        /*	rp_update_renderer ();
        	rp_stop_renderer ();*/
}

void pop_msg_box ( char *content, GtkWidget *window, GtkMessageType type )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( GTK_WINDOW ( window ), GTK_DIALOG_MODAL,
                                          type, GTK_BUTTONS_CLOSE, "%s", content );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

void update_current_info_bridge ( void )
{
#if 0
        struct alg_named_params *pa = &g_info_bridge->params;

        /* widgets: window, draw region */
        push_named_params ( g_main_win.window, "Main-Window-Widget", pa );
        push_named_params ( g_main_win.draw_region, "Draw-Region-Widget", pa );

        /* main window size */
        gint width, height;
        gtk_window_get_size ( GTK_WINDOW ( g_main_win.window ), &width, &height );
        push_named_params ( width, "Main-Window-Width", pa );
        push_named_params ( height, "Main-Window-Height", pa );

        /* draw region size and position */
        GtkAllocation *allocation = g_new0 ( GtkAllocation, 1 );
        gtk_widget_get_allocation ( g_main_win.draw_region, allocation );
        width = allocation->width;
        height = allocation->height;
        gint x, y;
        gtk_widget_translate_coordinates ( g_main_win.draw_region,
                                           g_main_win.win_box, 0, 0, &x, &y );
        push_named_params ( width, "Draw-Region-Width", pa );
        push_named_params ( height, "Draw-Region-Height", pa );
        push_named_params ( x, "Draw-Region-X", pa );
        push_named_params ( y, "Draw-Region-Y", pa );

        /* Rendering settings */
        static int use_preset = 1;
        if ( use_preset ) {
                /* First run */
                push_named_params ( RENDERER_RASTERIZER, "Renderer-Type", pa );
                use_preset = 0;
        } else {
        }
#endif
}

gboolean display_logo_callback ( GtkWidget *draw_region, cairo_t *cairo, gpointer data )
{
        gdk_cairo_set_source_pixbuf ( cairo, g_main_win.logo_pix_buf, 0, 0 );
        cairo_paint ( cairo );
        cairo_fill ( cairo );
        return 0;
}

void edit_main_loop ( void )
{
        gtk_main ();
}
