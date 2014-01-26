// RenderProcedure.cpp: Contains the primary rendering procedure
#include "render_procedure.h"

#if 0

core_handle_t g_core = X3D_CORE_CONTEXT_INVALID;
scene_handle_t g_scene = X3D_SCENE_INVALID;
renderer_handle_t g_renderer = X3D_RENDERER_INVALID;

pthread_t g_rend_thread = nullptr;
int togo = 0;

void *render_thread ( void *data );
void *test_thread ( void *data );


void rp_init ( void )
{
	g_core = create_core_context_x3d ();
}

void rp_free ( void )
{
	if ( g_core != X3D_CORE_CONTEXT_INVALID ) {
		free_core_context_x3d ();
	}
}

void rp_new_scene ()
{
	g_scene = create_scene_x3d ();
}

enum RP_SCENE_ERR rp_load_scene ( char *filename )
{
	scene_handle_t new_scene = load_scene_from_file_x3d ( filename );
	if ( new_scene == X3D_SCENE_INVALID ) {
		return RP_SCENE_LOAD_FAILED;
	}
	if ( g_scene != X3D_SCENE_INVALID ) {
		switch_scene_x3d ( g_scene );
		free_scene_x3d ();
	}
	g_scene = new_scene;
	switch_scene_x3d ( new_scene );
	build_scene_x3d ();
	return RP_SCENE_NORMAL;
}

enum RP_SCENE_ERR rp_unload_scene ()
{
	if ( g_scene == X3D_SCENE_INVALID ) {
		return RP_SCENE_NONEXIST;
	}
	free_scene_x3d ();
	g_scene = X3D_SCENE_INVALID;
	return RP_SCENE_NORMAL;
}

int rp_load_model ( char *filename )
{
	begin_edit_scene_x3d ();
	load_model_from_file_x3d ( filename );
	end_edit_scene_x3d ();
}

int rp_unload_model ( char *name )
{
	begin_edit_scene_x3d ();
	/* unload_model_scene_x3d ( mh ); */
	end_edit_scene_x3d ();
}

void rp_update_renderer ( void )
{
	struct info_bridge *ib = get_info_bridge ();
	if ( g_renderer == X3D_RENDERER_INVALID ) {
		enum RENDERER_IDR rend_type;
		pop_named_params_x3d ( ib->params, "Renderer-Type", rend_type );
		g_renderer = create_renderer_x3d ( rend_type );
		switch_renderer_x3d ( g_renderer );
	}
	begin_edit_renderer_x3d ();
	end_edit_renderer_x3d ();
}

void rp_render ( void )
{
	pthread_attr_t attr;
	pthread_attr_init ( &attr );
	pthread_create ( &g_rend_thread, &attr, render_thread, nullptr );
	togo = 1;
}

void *render_thread ( void *data )
{
	if ( g_core == X3D_CORE_CONTEXT_INVALID ||
	     g_renderer == X3D_RENDERER_INVALID ||
	     g_scene == X3D_SCENE_INVALID ) {
		return nullptr;
	}
	switch_core_context_x3d ( g_core );
	switch_renderer_x3d ( g_renderer );
	switch_scene_x3d ( g_scene );
	struct info_bridge *ib = get_info_bridge ();
	/** FIXME: check info **/
	int i;
	int end = 1;		/* hardcode for current time */
	for ( i = 0; i != end && togo == 1; i ++ ) {
		begin_renderer_x3d ();
		draw_scene_renderer_x3d ();
		end_renderer_x3d ();
	}
	return nullptr;
}

void rp_stop_renderer ( void )
{
	togo = 0;
	stop_renderer_x3d ();
}

void *test_thread ( void *data )
{
	struct info_bridge *ib = get_info_bridge ();
	int i;
	int end = 1;		/* hardcode for current time */
	for ( i = 0; i != end && togo == 1; i ++ ) {
		begin_renderer_x3d ();
		draw_scene_renderer_x3d ();
		end_renderer_x3d ();
	}
	return nullptr;
}

void rp_test ( void )
{
	debug_setup_x3d ( "resource_piler_test" );
	/*	pthread_attr_t attr;
		pthread_attr_init ( &attr );
		pthread_create ( &g_rend_thread, &attr, test_thread, nullptr );*/
	togo = 1;
	test_thread ( nullptr );
}

#endif // 0
