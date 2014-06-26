#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <gtk/gtk.h>
#include <usr/usr_editorfrontend.hpp>

struct path_resource {
        char *glade_path;
        char *logo_file;
        char *raw_media_dir;
        char *config_dir;
};


static const struct path_resource g_path_res = {
        .glade_path = (char *) "./etc/editor/",
        .logo_file = (char *) "./etc/editor/x3d_logo.png",
        .raw_media_dir = (char *) "./etc/media/",
        .config_dir = (char *) "./etc/config/"
};

/*
 * functions' declaration
 */
string file_chooser_open ( string default_dir );
string file_chooser_save ( string default_dir );
void message_box_info ( string title, string message );
void message_box_warning ( string title, string message );
void message_box_error ( string title, string message );
bool message_box_question ( string title, string message );

void widget_get_size ( GtkWidget* parent, GtkWidget* widget,
                       int* x, int* y, int* width, int* height );
GtkBuilder* builder_load ( string filename );
void builder_all_set ( GtkBuilder* builder );
void run_gtk_main ( void );
void stop_gtk_main ( void );
void await_gtk_main ( void );


#endif // GUI_H_INCLUDED
