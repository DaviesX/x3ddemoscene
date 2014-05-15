#ifndef MAIN_EDITOR_H_INCLUDED
#define MAIN_EDITOR_H_INCLUDED

/*
 * functions' declaration
 */
bool main_editor_load ( char *glade_path );
void main_editor_set ( int state );

GtkWidget *main_editor_get_region ( void );
void main_editor_draw_tmp_image ( void );


#endif // MAIN_EDITOR_H_INCLUDED
