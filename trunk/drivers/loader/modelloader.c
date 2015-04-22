/* modelloader.c: All model loading functions go here */
#include <container/staging.h>
#include <misc.h>
#include <logout.h>
#include <x3d/resloader.h>
#include <loader/modelloader.h>
#include "modelloader.h"
#include "objmodel.h"
#include "pbrtmodel.h"
#include "fbxmodel.h"


// Global variables
int autoSceneId = SCENE_ID_INVALID;
int autoModelId = MODEL_ID_INVALID;

int load_scene_from_x3dscene ( char *filename, struct stager *stg );
int load_scene_from_pbrt ( char *filename, struct stager *stg );
int load_model_file ( char *filename, entry_rec_t *rec );


/* Load scene to the resource piler from file */
int load_scene_from_file ( char *filename, struct stager *stg )
{
        int scene_id;
        if ( alg_match_substring ( filename, ".X3dScene" ) ) {
                scene_id = load_scene_from_x3dscene ( filename, stg );
        } else if ( alg_match_substring ( filename, ".pbrt" ) ) {
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
        entry_rec_t *rec = create_entry_record ( stg );
        make_scene_str ( scene_id, buff );
        stager_enter_root ( buff, rec );
        stager_set_layer_first ( rec );
        make_model_str ( model_id, buff );
        int rc = stager_begin_entry ( buff, rec );
//        if ( !load_model_file ( filename, rec ) ) {
        log_severe_err_dbg ( "LoadModelFromFile",
                             "Failed to load model file, file name: %s", filename );
        return MODEL_ID_INVALID;
//        }
        stager_end_entry ( rc, rec );
        free_entry_record ( rec );
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

#if 0
/* Load a model to resource piler from file */
int load_model_file ( char *filename, entry_rec_t *rec )
{
        int err;
//	struct stager *stg = nullptr;
        if ( alg_match_substring ( filename, ".obj" ) ) {
                err = LoadModelFileObj ( filename, rec );
        } else if ( alg_match_substring ( filename, ".pbrt" ) ) {
                err = LoadModelFilePbrt ( filename, rec );
        } else if ( alg_match_substring ( filename, ".fbx" ) ) {
                err = LoadModelFileFbx ( filename, rec );
        }
        return err;
}
#endif // 0

bool load_model_entity ( char *filename, struct res_task *task )
{
        bool err;
        if ( alg_match_substring ( filename, ".obj" ) ) {
                err = load_obj_model ( filename, task );
        } else if ( alg_match_substring ( filename, ".pbrt" ) ) {
                err = load_pbrt_model ( filename, task );
        } else if ( alg_match_substring ( filename, ".fbx" ) ) {
                err = load_fbx_model ( filename, task );
        } else if ( alg_match_substring ( filename, ".x" ) ) {
        } else if ( alg_match_substring ( filename, ".sdkmesh" ) ) {
        } else if ( alg_match_substring ( filename, ".blend" ) ) {
        } else {
                log_severe_err_dbg ( "unsupported format: %s", filename );
                err = false;
        }
        return err;
}

bool save_model_entity ( char *filename, struct res_comp *comp )
{
        return true;
}

bool free_model_entity ( struct res_task *task )
{
        return true;
}
