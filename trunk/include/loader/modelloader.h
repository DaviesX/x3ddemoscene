#ifndef MODELLOADER_H_INCLUDED
#define MODELLOADER_H_INCLUDED


/*
 * functions' declaration
 */
/*

bool load_x_model ( char *filename, struct res_task *task );
bool load_sdkmesh_model ( char *filename, struct res_task *task );
bool load_blend_model ( char *filename, struct res_task *task );


bool free_x_model ( struct res_task *task );
bool free_sdkmesh_model ( struct res_task *task );
bool free_blend_model ( struct res_task *task );
*/

bool load_model_entity ( char *filename, struct res_task *task );
bool save_model_entity ( char *filename, struct res_comp *comp );
bool free_model_entity ( struct res_task *task );


#endif // MODELLOADER_H_INCLUDED
