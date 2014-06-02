/* editrenderregion.c: render region activex implementation */
#include <thread.h>
#include <x3d/editor.h>


struct renderer;

struct ax_render_region {
        struct edit_activex _parent;

        enum PLATFORM_IDR type;
        void *handle;
        struct irectangle2d rect;
        struct renderer *rend;
        bool is_idle;
        bool is_resized;

        struct thr_trap block_driver;
        int iswap;
        struct {
                bool is_idle;
                f_Notify_idle f_notify_idle;
                void *d_notify_idle;
        } idle[2];
        struct {
                bool is_resized;
                bool is_fullscreen;
                int width;
                int height;
                f_Notify_resize f_notify_resize;
                void *d_notify_resize;
        } resize[2];
};


__dlexport struct ax_render_region *create_ax_render_region (
        enum PLATFORM_IDR type, void *handle, int x, int y, int w, int h )
{
        struct ax_render_region *region = alloc_fix ( sizeof *region, 1 );
        edit_activex_init ( EDIT_ACTIVEX_RENDER_REGION, (struct edit_activex *) region );

        thr_init_trap ( &region->block_driver );
        region->type = type;
        region->handle = handle;
        build_irectangle_2d ( x, y, x + w, y + h, &region->rect );
        region->rend = nullptr;
        region->is_idle = true;
        region->is_resized = false;
        region->iswap = 0;
        int i;
        for ( i = 0; i < 2; i ++ ) {
                region->idle[i].is_idle = true;
                region->idle[i].f_notify_idle = nullptr;
                region->idle[i].d_notify_idle = nullptr;

                region->resize[i].is_resized = false;
                region->resize[i].f_notify_resize = nullptr;
                region->resize[i].d_notify_resize = nullptr;
                region->resize[i].width = w;
                region->resize[i].height = h;
                region->resize[i].is_fullscreen = false;
        }
        return region;
}

void ax_render_region_notify_idle ( bool is_idle, struct ax_render_region *region )
{
        int i = region->iswap & 1;
        region->is_idle = is_idle;
        region->idle[i].is_idle = is_idle;
}

__dlexport void ax_render_region_set_renderer (
        struct renderer *rend, struct ax_render_region *region )
{
        region->rend = rend;
}

__dlexport void ax_render_region_resize ( int x, int y, int w, int h, bool toggle_fullscreen,
                                          struct ax_render_region *region )
{
        int i = region->iswap & 1;
        region->resize[i].is_resized = true;
        region->resize[i].width = w;
        region->resize[i].height = h;
        region->resize[i].is_fullscreen = toggle_fullscreen;
        build_irectangle_2d ( x, y, x + w, y + h, &region->rect );
        region->is_resized = true;
}

__dlexport void ax_render_region_bind_signal ( char *signal, f_Generic func, void *info,
                                               struct ax_render_region *region )
{
        int i = region->iswap & 1;

        if ( !strcmp ( "notify_idle", signal ) ) {
                region->idle[i].f_notify_idle = (f_Notify_idle) func;
                region->idle[i].d_notify_idle = info;
        } else if ( !strcmp ( "notify_resize", signal ) ) {
                region->resize[i].f_notify_resize = (f_Notify_resize) func;
                region->resize[i].d_notify_resize = info;
        }
}

void ax_render_region_dispatch ( struct ax_render_region *region )
{
        thr_trap_on_task ( &region->block_driver );
        int i = (region->iswap + 1) & 1;
        {
                if ( region->idle[i].f_notify_idle ) {
                        region->idle[i].f_notify_idle ( region->idle[i].is_idle,
                                                region->handle,
                                                region->idle[i].d_notify_idle );
                }
        }
        {
                if ( region->resize[i].f_notify_resize ||
                     region->resize[i].is_resized ) {
                        region->resize[i].f_notify_resize ( region->resize[i].width,
                                                            region->resize[i].height,
                                                            region->resize[i].is_fullscreen,
                                                            region->handle,
                                                            region->resize[i].d_notify_resize );
                        region->resize[i].is_resized = false;
                }
        }
        thr_untrap_task ( &region->block_driver );
}

void ax_render_region_update ( struct ax_render_region *region )
{
        thr_trap_on_task ( &region->block_driver );
        region->iswap ++;
        thr_untrap_task ( &region->block_driver );

        if ( !region->is_idle ) {
                renderer_commit ( region->rend );
        }
        if ( region->is_resized ) {
                region->is_resized = false;
        }
}

void ax_render_region_load_state ( struct serializer *s, struct ax_render_region *region )
{
}

void ax_render_region_save_state ( struct serializer *s, struct ax_render_region *region )
{
}

void ax_render_region_free ( struct ax_render_region *region )
{
        memset ( region, 0, sizeof *region );
        free_fix ( region );
}
