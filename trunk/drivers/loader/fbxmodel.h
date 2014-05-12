#ifndef FBX_MODEL_H_INCLUDED
#define FBX_MODEL_H_INCLUDED

#include <x3d/resloader.h>


/*
 * Functions' declaration
 */

// Load a model from .fbx format file
bool load_fbx_model ( char *filename, struct res_task *task );
bool free_fbx_model ( struct res_task *task );


#endif // FBX_MODEL_H_INCLUDED
