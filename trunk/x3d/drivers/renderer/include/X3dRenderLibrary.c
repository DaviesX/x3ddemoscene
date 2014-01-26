/* X3dRenderLibrary.c: General Interface Implementation */
#include "X3dRenderLibrary.h"


struct core_context {
	int state;
	struct {
		/* Renderer info */
		renderer_handle_t curr_rend;
		struct alg_list rend_list;
	};
	struct {
		/* Scene info */
		scene_handle_t curr_scene;
		struct alg_list scene_list;
	};
	struct {
		/* System info */
	};
	struct {
		/* Resource info */
		struct resource_piler dynamic_res;
		struct resource_piler static_res;
	};
	struct {
		/* Global variables */
		x3d_named_params_t *g_var;
	};
};

#define MAX_CORE_CONTEXT		64
#define CORE_STATE_UNUSE		0
#define CORE_STATE_INUSE		1

struct core_context g_core_context[MAX_CORE_CONTEXT] = {0};
int g_num_core_context = 0;
core_handle_t g_default_core = X3D_CORE_CONTEXT_INVALID;

#define i_core( _i )		(g_core_context[_i])
#define check_core( _i ) \
{ \
	log_normal_dbg ( "Checking core context ..." ); \
	assert ( _i >= 0 && _i < MAX_CORE_CONTEXT ); \
	assert ( i_core ( _i ).state == CORE_STATE_INUSE ); \
}


core_handle_t create_core_context_x3d ( void )
{
	/* Initialize log reporter */
	init_log_output ( 1 );

	/* Lookup if there is a free slot to put in */
	int iavail = 0;
	while ( 1 ) {
		if ( iavail >= MAX_CORE_CONTEXT ) {
			log_severe_err_dbg ( "No slot avialiable slot, sugguest to release other render coresd" );
			return X3D_CORE_CONTEXT_INVALID;
		} else {
			if ( i_core ( iavail ).state == CORE_STATE_UNUSE ) {
				log_normal_dbg ( "Valid slot is found, slot handle: %d", iavail );
				break;
			}
		}
		iavail ++;
	}

	memset ( &i_core ( iavail ), 0, sizeof ( struct core_context ) );

	i_core ( iavail ).curr_rend = X3D_RENDERER_INVALID;
	i_core ( iavail ).curr_scene = X3D_SCENE_INVALID;

	/* Initialize maths library */
	BuildSinCosTable ();
	BuildLog2BaseTable ();
	BuildPowTable ();

	create_alg_list ( &i_core ( iavail ).rend_list, sizeof ( struct renderer ), 0 );
	create_alg_list ( &i_core ( iavail ).scene_list, sizeof ( struct scene ), 0 );
	init_resource_piler ( &i_core ( iavail ).dynamic_res );
	init_resource_piler ( &i_core ( iavail ).static_res );
	i_core ( iavail ).state = CORE_STATE_INUSE;
	g_default_core = iavail;
	g_num_core_context ++;

	log_normal_dbg ( "Core context created, core handle: %x, number cores: %d",
			 iavail, g_num_core_context );
	return iavail;
}

void free_core_context_x3d ( void )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	free_resource_piler ( &i_core ( ch ).dynamic_res );
	free_resource_piler ( &i_core ( ch ).static_res );
	free_alg_list ( &i_core ( ch ).rend_list );
	free_alg_list ( &i_core ( ch ).scene_list );
	memset ( &i_core ( ch ), 0 , sizeof ( struct core_context ) );
	i_core ( ch ).state = CORE_STATE_UNUSE;
	g_num_core_context --;

	log_normal_dbg ( "Core context released, core handle: %x, number cores: %d",
			 ch, g_num_core_context );
	free_log_output ();
}

void switch_core_context_x3d ( core_handle_t ch )
{
	check_core ( ch );
	g_default_core = ch;
}

#define cmp_renderer( _info, _elm )	( *(_info) == ((struct renderer *) (_elm))->rh )
#define cmp_scene( _info, _elm )	( *(_info) == ((struct scene *) (_elm))->sh )

void switch_renderer_x3d ( renderer_handle_t rh )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	DEBUG_SESSION (
		int pos;
		find_elm_alg_list ( &i_core ( ch ).rend_list, &rh, &pos, cmp_renderer );
		assert ( pos != -1 );
	);
	i_core ( ch ).curr_rend = rh;
}

void switch_scene_x3d ( scene_handle_t sh )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	DEBUG_SESSION (
		int pos;
		find_elm_alg_list ( &i_core ( ch ).scene_list, &sh, &pos, cmp_renderer );
		assert ( pos != -1 );
	);
	i_core ( ch ).curr_scene = sh;
}

x3d_named_params_t *get_global_core_context ( void )
{
	check_core ( g_default_core );
	return i_core ( g_default_core ).g_var;
}

struct renderer *get_renderer_core_context ( renderer_handle_t *hrend )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	renderer_handle_t rh = i_core ( ch ).curr_rend;
	int pos;
	find_elm_alg_list ( &i_core ( ch ).rend_list, &rh, &pos, cmp_renderer );
	struct renderer *r;
	if ( pos != -1 ) {
		r = alg_list_i ( &i_core ( ch ).rend_list, pos );
	} else {
		r = nullptr;
	}
	if ( hrend != nullptr ) {
		*hrend = rh;
	}
	return r;
}

struct scene *get_scene_core_context ( scene_handle_t *hscene )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	scene_handle_t sh = i_core ( ch ).curr_scene;
	int pos;
	find_elm_alg_list ( &i_core ( ch ).scene_list, &sh, &pos, cmp_scene );
	struct scene *s;
	if ( pos != -1 ) {
		s = alg_list_i ( &i_core ( ch ).scene_list, pos );
	} else {
		s = nullptr;
	}
	if ( hscene != nullptr ) {
		*hscene = sh;
	}
	return s;
}

void push_renderer_core_context ( struct renderer *r )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	add_element_alg_list ( r, &i_core ( ch ).rend_list );
}

void push_scene_core_context ( struct scene *s )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	add_element_alg_list ( s, &i_core ( ch ).scene_list );
}

void pop_renderer_core_context ( renderer_handle_t rh )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	remove_elm_alg_list ( &i_core ( ch ).rend_list, &rh, cmp_renderer );
}

void pop_scene_core_context ( scene_handle_t sh )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	remove_elm_alg_list ( &i_core ( ch ).scene_list, &sh, cmp_scene );
}

#undef cmp_scene
#undef cmp_renderer

struct resource_piler *get_dynamic_res_core_context ( void )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	return &i_core ( ch ).dynamic_res;
}

struct resource_piler *get_static_res_core_context ( void )
{
	check_core ( g_default_core );
	core_handle_t ch = g_default_core;
	return &i_core ( ch ).static_res;
}

/* This will cause a bypassing of all the settings by using a preset
 * and then jump to the test cases created internally */
void debug_setup_x3d ( char *test_case )
{
	check_core ( g_default_core );
	renderer_handle_t rh;
	scene_handle_t sh;
	struct renderer *r = get_renderer_core_context ( &rh );
	struct scene *s = get_scene_core_context ( &sh );
	dbg_setup ( test_case, r, s );
}

/* This will run the test case directly without going through the renderer */
void debug_run_x3d ( char *test_case )
{
	dbg_direct_test ( test_case );
}
