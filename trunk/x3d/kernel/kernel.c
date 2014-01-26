#include <x3d/common.h>
#include <math/math.h>
#include <algorithm.h>
#include <logout.h>
#include <x3d/kernel.h>
#include <x3d/runtime_debug.h>
#include <x3d/resource_loader.h>
#include <x3d/renderer.h>
#include <x3d/scene.h>
#include <x3d/editor.h>


struct runtime_debug {
	struct alg_list test_case;
	struct alg_list case_name;
	struct alg_named_params dbg_info;
};

struct global_lib {
	struct alg_list lib_func;
};

struct kernel {
	int argc;
	char **argv;
	struct editor *edit;
	struct renderer_context *rend_con;
	struct scene *scene;
	struct runtime_debug rt_debug;
	struct global_lib gb_lib;
};

static struct kernel g_kernel;

static void rest_init ( void );
static void kernel_loop ( void );
static void init_test_case ( void );
static void invoke_test_case ( enum DBG_POSITION pos );
static void gen_debug_info ( void );

void start_kernel ( void )
{
	init_test_case ();
	init_math_lib ();
	init_log_output ( true );
	init_res_loader ();
	init_renderer_context ();
	g_kernel.rend_con = export_renderer_context ();
	init_editor ( &g_kernel.argc, &g_kernel.argv );
	g_kernel.edit = export_editor ();

	rest_init ();
}

void shutdown_kernel ( void )
{
}

static void rest_init ( void )
{
	invoke_test_case ( DBG_KERNEL_START );
}

static void kernel_loop ( void )
{
}

void init_startup_param ( int argc, char **argv )
{
	g_kernel.argc = argc;
	g_kernel.argv = alloc_var ( sizeof ( char * ), argc );
	int i;
	for ( i = 0; i < argc; i ++ ) {
		g_kernel.argv[i] = argv[i];
	}
}

void add_startup_param ( char *option, char *param )
{
	add_var ( g_kernel.argv, 2 );
	int n = g_kernel.argc;
	g_kernel.argv[n + 0] =
		alloc_fix ( sizeof ( char ), strlen ( option ) + 1 );
	strcpy ( g_kernel.argv[n + 0], option );
	g_kernel.argv[n + 1] =
		alloc_fix ( sizeof ( char ), strlen ( param ) + 1 );
	strcpy ( g_kernel.argv[n + 1], param );
	g_kernel.argc += 2;
}

static void init_test_case ( void )
{
	struct runtime_debug *dbg = &g_kernel.rt_debug;
	create_alg_list ( &dbg->test_case, sizeof ( struct unit_test ), 0 );
	create_alg_list ( &dbg->test_case, sizeof ( char* ), 0 );
	int i;
	for ( i = 0; i < g_kernel.argc - 1; i ++ ) {
		if ( !strcmp ( "--test", g_kernel.argv[i] ) ) {
			char *curr = g_kernel.argv[i + 1];
			char *casename = alloc_fix ( sizeof casename, strlen ( curr ) + 1 );
			strcpy ( casename, curr );
			add_element_alg_list ( casename, &dbg->case_name );
		}
	}
}

static void invoke_test_case ( enum DBG_POSITION pos )
{
	gen_debug_info ();

	struct runtime_debug *dbg = &g_kernel.rt_debug;
	char *curr = alg_list_first ( &dbg->case_name );
	struct {
		char *name;
		enum DBG_POSITION pos;
	} info;
	int i;
	for ( i = 0; i < alg_list_len ( &dbg->case_name ); i ++ ) {
		info.name = curr;
		info.pos = pos;
#define cmp( _info, _elm )	(((struct unit_test *) _elm)->pos & (_info)->pos && \
				 !strcmp ( (_info)->name, ((struct unit_test *) _elm)->test_name ))
		int i_case;
		find_elm_alg_list ( &dbg->test_case, &info, &i_case, cmp )
#undef cmp
		if ( i_case != -1 ) {
			struct unit_test *test = alg_list_i ( &dbg->test_case, i_case );
			test->test ( &dbg->dbg_info );
		} else {
			log_normal_dbg ( "current case: not involve in %x", curr, pos );
		}
	}
}

static void gen_debug_info ( void )
{
	struct runtime_debug *dbg = &g_kernel.rt_debug;
	reset_named_params ( &dbg->dbg_info );
	push_named_params ( g_kernel.argc, "argc", &dbg->dbg_info );
	push_named_params ( g_kernel.argv, "argv", &dbg->dbg_info );
	push_named_params ( g_kernel.edit, "editor", &dbg->dbg_info );
	push_named_params ( g_kernel.rend_con, "renderer-context", &dbg->dbg_info );
	push_named_params ( g_kernel.scene, "scene", &dbg->dbg_info );
}

void kernel_unit_test_add ( struct unit_test *ut )
{
	add_element_alg_list ( ut, &g_kernel.rt_debug.test_case );
}

void kernel_global_lib_add ( struct global_lib *lib )
{
}

void kernel_lib_func_retrieve ( char *func_name, struct lib_func *func )
{
}
