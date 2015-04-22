/* fbxmodel.c: FBX model file loading functions all go here */
#include <container/staging.h>
#include <logout.h>
#include "fbxmodel.h"


// Load a model from .fbx format file
bool load_fbx_model ( char *filename, struct res_task *task )
{
        log_severe_err_dbg ( "LoadModelFileFbx",
                             "This block is not implemented yet" );
        return false;
}

bool free_fbx_model ( struct res_task *task )
{
        return false;
}
