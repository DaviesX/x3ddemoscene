/* editrenderregion.c: render region activex implementation */
#include <x3d/editor.h>


struct activex_render_region {
        struct edit_activex activex;
        enum PLATFORM_IDR type;
        void *handle;
        struct irectangle2d rect;
        uuid_t rend_bind;
};


__dlexport struct activex_render_region *create_activex_render_region (
        enum PLATFORM_IDR type, void *handle, int x, int y, int w, int h )
{
        struct activex_render_region *rr = alloc_fix ( sizeof *rr, 1 );
        edit_activex_init ( EDIT_ACTIVEX_RENDER_REGION, cast(&rr->activex) rr );
        rr->type = type;
        rr->handle = handle;
        rr->rect.x0 = x;
        rr->rect.y0 = y;
        rr->rect.x1 = x + w;
        rr->rect.y1 = y + h;
        rr->rend_bind = renderer_container_add ( nullptr );
        return rr;
}

void activex_render_region_update ( struct activex_render_region *activex )
{
}

void activex_render_region_free ( struct activex_render_region *activex )
{
}
