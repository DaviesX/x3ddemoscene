/* pbrt_model.cpp: pbrt model file loading functions all go here */
#include <staging.h>
#include <logout.h>
#include "pbrtmodel.h"



// Load a model from pbrt format geomtry file
bool load_pbrt_model ( char *filename, struct res_task *task )
{
        log_severe_err_dbg ( "LoadModelFilePbrt",
                             "This block is not implemented yet" );
        return false;
}

bool free_pbrt_model ( struct res_task *task )
{
        return false;
}
