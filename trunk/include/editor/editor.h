#ifndef EDITOR_DRIVER_H_INCLUDED
#define EDITOR_DRIVER_H_INCLUDED


#include <x3d/editor.h>

/*
 * Functions' declarations
 */
bool editor_init ( int *argc, char ***argv );
bool editor_end_init ( void );
bool editor_load ( void );
void editor_loop ( void *info );
bool editor_free ( void );

void *dbg_get_render_region ( void );
void dbg_hand_image_to_display ( void *ptr, int w, int h );


#endif // EDITOR_DRIVER_H_INCLUDED
