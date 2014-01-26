#ifndef X3DRENDERLIBRARY_H_INCLUDED
#define X3DRENDERLIBRARY_H_INCLUDED


/* Commons */
#include "../Rendering/X3dInfrastructureLayer.h"
#include "../Rendering/X3dComponentLayer.h"
#include "../Rendering/X3dCompositionLayer.h"

#define X3D_LIB_VERSION_STRING			RENDER_LIB_VERSION_STRING
#define X3D_CORE_CONTEXT_INVALID		-1
#define X3D_RENDERER_INVALID			-1
#define X3D_SCENE_INVALID			-1

typedef uint32_t core_handle_t;

/*
 * Includes relative headers
 */
#include "IX3dUtility.h"
#include "IX3dScene.h"
#include "IX3dRendering.h"


/* Functions' declaration */
core_handle_t create_core_context_x3d ( void );
void free_core_context_x3d ( void );
void switch_core_context_x3d ( core_handle_t ch );
void switch_renderer_x3d ( renderer_handle_t rh );
void switch_scene_x3d ( scene_handle_t sh );

void debug_setup_x3d ( char *test_case );
void debug_run_x3d ( char *test_case );

x3d_named_params_t *get_global_core_context ( void );
struct renderer *get_renderer_core_context ( renderer_handle_t *hrend );
struct scene *get_scene_core_context ( scene_handle_t *hscene );
struct resource_piler *get_dynamic_res_core_context ( void );
struct resource_piler *get_static_res_core_context ( void );

void push_renderer_core_context ( struct renderer *r );
void push_scene_core_context ( struct scene *s );
void pop_renderer_core_context ( renderer_handle_t rh );
void pop_scene_core_context ( scene_handle_t sh );


#endif // X3DRENDERLIBRARY_H_INCLUDED
