#ifndef MAIN_EDITOR_H_INCLUDED
#define MAIN_EDITOR_H_INCLUDED

/*
 * functions' declaration
 */

GtkWidget *main_editor_get_region ( void );
void main_editor_draw_tmp_image ( void );

// Gtk callbacks
extern "C" gboolean destroy_callback ( GtkWidget *widget, gpointer data );
extern "C" gboolean main_editor_dispatch ( gpointer user_data );


#endif // MAIN_EDITOR_H_INCLUDED
