#ifndef EDITOR_DRIVER_H_INCLUDED
#define EDITOR_DRIVER_H_INCLUDED


#include <x3d/editor.h>

/*
 * Functions' declarations
 */
bool load_editor ( int *argc, char ***argv );
void edit_main_loop ( void );

void *dbg_get_render_region ( void );
void dbg_hand_image_to_display ( void *ptr, int w, int h );


#endif // EDITOR_DRIVER_H_INCLUDED
