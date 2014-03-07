/* model_loader.c: All model loading functions go here */
#include <staging.h>
#include <logout.h>
#include "model_loader.h"
#include "obj_model.h"
#include "pbrt_model.h"
#include "fbx_model.h"


// Global variables
int autoSceneId = SCENE_ID_INVALID;
int autoModelId = MODEL_ID_INVALID;

int load_scene_from_x3dscene ( char *filename, struct stager *stg );
int load_scene_from_pbrt ( char *filename, struct stager *stg );
int load_model_file ( char *filename, res_record_t *rec );


/* Load scene to the resource piler from file */
int load_scene_from_file ( char *filename, struct stager *stg )
{
        int scene_id;
        if ( MatchSubStr ( filename, ".X3dScene" ) ) {
                scene_id = load_scene_from_x3dscene ( filename, stg );
        } else if ( MatchSubStr ( filename, ".pbrt" ) ) {
                scene_id = load_scene_from_pbrt ( filename, stg );
        } else {
                log_severe_err_dbg ( "LoadSceneFromFile",
                                     "Unsupported scene file, file name: %s", filename );
                return SCENE_ID_INVALID;
        }
        return scene_id;
}

/* Load model to the resource piler from file */
int load_model_from_file ( char *filename, int scene_id, struct stager *stg )
{
        char buff[64];
        int model_id = alloc_model_id ();
        /* Enter the current scene, and make a new entry for model to load in */
        res_record_t *rec = create_res_record ( stg );
        make_scene_str ( scene_id, buff );
        enter_root_stager ( buff, rec );
        set_layer_first_stager ( rec );
        make_model_str ( model_id, buff );
        int rc = begin_entry_stager ( buff, rec );
        if ( !load_model_file ( filename, rec ) ) {
                log_severe_err_dbg ( "LoadModelFromFile",
                                     "Failed to load model file, file name: %s", filename );
                return MODEL_ID_INVALID;
        }
        end_entry_stager ( rc, rec );
        free_res_record ( rec );
        return model_id;
}

/* Load a X3dScene format scene file */
int load_scene_from_x3dscene ( char *filename, struct stager *stg )
{
        return 0;
}

/* Load a Pbrt format scene file */
int load_scene_from_pbrt ( char *filename, struct stager *stg )
{
        return 0;
}

/* Load a model to resource piler from file */
int load_model_file ( char *filename, res_record_t *rec )
{
        int err;
//	struct stager *stg = nullptr;
        if ( MatchSubStr ( filename, ".obj" ) ) {
                err = LoadModelFileObj ( filename, rec );
        } else if ( MatchSubStr ( filename, ".pbrt" ) ) {
                err = LoadModelFilePbrt ( filename, rec );
        } else if ( MatchSubStr ( filename, ".fbx" ) ) {
                err = LoadModelFileFbx ( filename, rec );
        }
        return err;
}
