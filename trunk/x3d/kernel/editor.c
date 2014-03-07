#include <logout.h>
#include <algorithm.h>
#include <x3d/kernel.h>
#include <editor/editor.h>
#include <x3d/renderer.h>
#include <x3d/editor.h>

struct editor {
        enum EDIT_MODE mode;
        struct alg_list rend_region;
        struct alg_llist llist;
};

static struct info_bridge g_info_bridge = {0};
static struct editor g_edit = {0};

static struct edit_ops g_edit_ops = {
        .init_editor = load_editor,
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
        create_alg_list ( &g_edit.rend_region, sizeof ( struct render_region ),
                          MAX_RENDER_REGIONS );
        create_alg_llist ( &g_edit.llist, MAX_RENDER_REGIONS );
        init_named_params ( &g_info_bridge.params );

        if ( !g_edit_ops.init_editor ( argc, argv, mode ) ) {
                goto fail;
        }
        return true;
fail:
        kernel_panic ();
        return false;
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

void render_region_add ( struct render_region *rr )
{
        if ( alg_list_len ( &g_edit.rend_region ) >= MAX_RENDER_REGIONS ) {
                log_severe_err_dbg ( "render region slot has been full" );
                memset ( rr, 0, sizeof *rr );
        }
        rr->rend_bind = renderer_context_add ( RENDERER_IDR_RASTERIZER );
        int pos = alg_llist_add ( &g_edit.llist );
        expand_alg_list ( pos + 1, &g_edit.rend_region );
        struct render_region *dst_rr = alg_list_i ( &g_edit.rend_region, pos );
        memcpy ( dst_rr, rr, sizeof *rr );
}

struct render_region *render_region_find ( struct render_region *rr )
{
}

void render_region_remove ( struct render_region *rr )
{
}

void render_region_flush ( void )
{
}
