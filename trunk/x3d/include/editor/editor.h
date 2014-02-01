#ifndef EDITOR_DRIVER_H_INCLUDED
#define EDITOR_DRIVER_H_INCLUDED


#include <x3d/editor.h>

/*
 * Functions' declarations
 */
bool load_editor ( int *argc, char ***argv, enum EDIT_MODE mode );
void edit_main_loop ( void );


#endif // EDITOR_DRIVER_H_INCLUDED
