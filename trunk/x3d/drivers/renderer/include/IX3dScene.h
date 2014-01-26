#ifndef IX3DSCENE_H_INCLUDED
#define IX3DSCENE_H_INCLUDED


/*
 * Functions' declaration
 */
scene_handle_t create_scene_x3d ( void );
scene_handle_t load_scene_from_file_x3d ( char *filename );
void build_scene_x3d ( void );
void free_scene_x3d ( void );

void begin_edit_scene_x3d ( void );
void end_edit_scene_x3d ( void );
model_handle_t load_model_from_file_x3d ( char *filename );
void unload_model_scene_x3d ( model_handle_t mh );
light_handle_t load_light_scene_x3d ( struct light *l );
void unload_light_scene_x3d ( light_handle_t lh );
camera_handle_t load_camera_scene_x3d ( struct camera *camera );
void unload_camera_scene_x3d ( camera_handle_t ch );
void set_static_struct_scene_x3d ( enum SCENE_STRUCT_IDR idr, uint32_t option );
void update_scene_x3d ( void );

struct model *get_model_scene_x3d ( model_handle_t mh );
struct light *get_light_scene_x3d ( light_handle_t lh );
struct camera *get_camera_scene_x3d ( camera_handle_t ch );


#endif // IX3DSCENE_H_INCLUDED
