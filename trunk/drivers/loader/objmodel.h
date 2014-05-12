#ifndef OBJLOADER_H_INCLUDED
#define OBJLOADER_H_INCLUDED

#include <x3d/resloader.h>

/*
 * Functions' declarations
 */
bool load_obj_model ( char *filename, struct res_task *task );
bool free_obj_model ( struct res_task *task );


#endif // OBJLOADER_H_INCLUDED
