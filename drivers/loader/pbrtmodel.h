#ifndef PBRT_MODEL_H_INCLUDED
#define PBRT_MODEL_H_INCLUDED

#include <x3d/resloader.h>


/*
 * Functions' declarations
 */

// Load a model from pbrt format geomtry file
bool load_pbrt_model ( char *filename, struct res_task *task );
bool free_pbrt_model ( struct res_task *task );


#endif // PBRT_MODEL_H_INCLUDED
