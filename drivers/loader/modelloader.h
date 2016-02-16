#ifndef X3DSCENELOADER_H_INCLUDED
#define X3DSCENELOADER_H_INCLUDED


#include <container/staging.h>

/*
 * Definitions
 */
#define SCENE_ID_INVALID				0
#define MODEL_ID_INVALID				0

#define SCENE_NAME_MAX_LENGTH				64


enum RESOURCE_ELM_IDR {
        /* Scene elements */
        RES_IDR_TRIANGLE_MESH,
        RES_IDR_TRIANGLE,
        RES_IDR_SPHERE,
        RES_IDR_CONE,
        RES_IDR_CYLINDER,
        RES_IDR_DISK,
        RES_IDR_PARABOLOID,
        RES_IDR_HYPERBOLOID,
        RES_IDR_PRIMITIVE,

        RES_IDR_VERTEX,
        RES_IDR_NORMAL,
        RES_IDR_TANGENT,
        RES_IDR_UV,

        RES_IDR_TEXTURE
};

/* Utility macro functions */
#define alloc_scene_id()		(-- autoSceneId)
#define alloc_model_id()		(-- autoModelId)
#define make_scene_str( _sceneId, _str ) \
	(sprintf ( (_str), "Scene:%x", (_sceneId) ))

#define make_model_str( _modelId, _str ) \
	(sprintf ( (_str), "Model:%x", (_modelId) ))

#define make_model_transform_str( _modelId, _str ) \
	(sprintf ( (_str), "Transform:%x", (_modelId) ))

#define make_model_material_str( _modelId, _str ) \
	(sprintf ( (_str), "Material:%x", (_modelId) ))

#define make_model_attribute_str( _modelId, _str ) \
	(sprintf ( (_str), "Attribute:%x", (_modelId) ))

/*
 * Structures
 */

/* Fixed length header information for any lists */
struct resource_header {
        enum RESOURCE_ELM_IDR elm_idr;
        int num_elm;
};

/*
 * Global Variable
 */
extern int autoSceneId;
extern int autoModelId;


/*
 * Functions' declaration
 */

int load_scene_from_file ( char *filename, struct stager *stg );
int load_model_from_file ( char *filename, int sceneId, struct stager *stg );


#endif // X3DSCENELOADER_H_INCLUDED
