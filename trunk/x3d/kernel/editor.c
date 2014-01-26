#include <algorithm.h>
#include <x3d/kernel.h>
#include <editor/gui.h>
#include <x3d/renderer.h>
#include <x3d/editor.h>

struct editor {
	enum EDIT_MODE mode;
	struct render_region *rr;
	struct alg_llist llist;
};

static struct info_bridge g_info_bridge = {0};
static struct editor g_edit = {0};

static struct edit_ops g_edit_ops = {
	.init_editor = load_main_window,
	.editor_main_loop = edit_main_loop
};

#define MAX_RENDER_REGIONS		16


bool init_editor ( int *argc, char ***argv )
{
	enum EDIT_MODE mode = EDIT_RUN_DEMO;
	int i;
	int n = *argc;
	char **params = *argv;
	for ( i = 0; i < n; i ++ ) {
		char *buff = params[i];
		if ( !strcmp ( buff, "--edit-mode" ) ) {
			mode = EDIT_RUN_EDITOR;
			break;
		}
	}
	if ( !g_edit_ops.init_editor ( argc, argv, mode ) ) {
		goto fail;
	}
	free_fix ( g_edit.rr );
	g_edit.rr = alloc_fix ( sizeof *g_edit.rr, MAX_RENDER_REGIONS );
	init_named_params ( &g_info_bridge.params )
	return true;
fail:
	kernel_panic ();
}

struct render_region *get_first_render_region ( int32_t *pos )
{
}

struct render_region *get_next_render_region ( int32_t *pos )
{
}

void update_editor ( void )
{
}

struct editor *export_editor ( void )
{
	return &g_edit;
}

void import_editor ( struct edit_ops *ops )
{
	memcpy ( &g_edit_ops, ops, sizeof ( *ops ) );
}

struct info_bridge *get_info_bridge ( void )
{
	return &g_info_bridge;
}

void add_render_region ( struct render_region *rr )
{
}

struct render_region *find_render_region ( struct render_region *rr )
{
}

void remove_render_region ( struct render_region *rr )
{
}

void flush_render_region ( void )
{
}
