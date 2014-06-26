/* editrenderregion.cpp: render region activex implementation */
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>

using namespace x3d;
using namespace x3d::usr;


class RenderRegionActiveX::RenderRegionInt
{
public:
        void*                   m_handle;
        struct irectangle2d     m_rect;
        struct renderer*        m_rend;
        bool                    m_is_idle;
        bool                    m_is_resized;

        struct thr_trap         m_block_driver;
        int                     m_iswap;

        struct Idle {
                bool            is_idle;
                f_Notify_idle   f_idle;
                void*           d_idle;
        } m_idle[2];

        struct Resize {
                bool            is_resized;
                bool            is_fullscreen;
                int             width;
                int             height;
                f_Notify_resize f_resize;
                void*           d_resize;
        } m_resize[2];
};


RenderRegionActiveX::RenderRegionActiveX ( string name, void *handle,
                                           int x, int y, int w, int h ) :
        EditorActiveX ( name, sizeof(RenderRegionActiveX), EDIT_ACTIVEX_RENDER_REGION )
{
        this->pimpl = new RenderRegionInt ();
        RenderRegionInt *inst = pimpl;

        x3d::thr_init_trap ( &inst->m_block_driver );
        x3d::build_irectangle_2d ( x, y, x + w, y + h, &inst->m_rect );
        inst->m_handle          = handle;
        inst->m_rend            = nullptr;
        inst->m_is_idle         = true;
        inst->m_is_resized      = false;
        inst->m_iswap           = 0;

        int i;
        for ( i = 0; i < 2; i ++ ) {
                inst->m_idle[i].is_idle = true;
                inst->m_idle[i].f_idle  = nullptr;
                inst->m_idle[i].d_idle  = nullptr;

                inst->m_resize[i].is_resized    = false;
                inst->m_resize[i].f_resize      = nullptr;
                inst->m_resize[i].d_resize      = nullptr;
                inst->m_resize[i].width         = w;
                inst->m_resize[i].height        = h;
                inst->m_resize[i].is_fullscreen = false;
        }
}

RenderRegionActiveX::~RenderRegionActiveX ()
{
        delete this->pimpl;
}

void RenderRegionActiveX::on_adding ( void )
{
        RenderRegionInt *inst = this->pimpl;
        KernelEnvironment *state = this->get_state_buffer ();
        state->declare ( c_WndHandle, inst->m_handle );
}

void RenderRegionActiveX::set_idle_state ( bool is_idle )
{
        int t;
        t = this->pimpl->m_iswap & 1;
        this->pimpl->m_is_idle = is_idle;
        this->pimpl->m_idle[t].is_idle = is_idle;
}

void RenderRegionActiveX::set_renderer ( struct renderer *renderer )
{
        this->pimpl->m_rend = renderer;
}

void RenderRegionActiveX::resize ( int x, int y, int w, int h, bool toggle_fullscreen )
{
        RenderRegionInt *inst = this->pimpl;
        int t;
        t = inst->m_iswap & 1;
        inst->m_resize[t].is_resized      = true;
        inst->m_resize[t].width           = w;
        inst->m_resize[t].height          = h;
        inst->m_resize[t].is_fullscreen   = toggle_fullscreen;
        inst->m_is_resized                = true;
        x3d::build_irectangle_2d ( x, y, x + w, y + h, &inst->m_rect );
}

void RenderRegionActiveX::bind_callback ( string signal, f_Generic callback, void *data )
{
        RenderRegionInt *inst = this->pimpl;
        int t;
        t = inst->m_iswap & 1;

        if ( "notify_idle" == signal ) {
                inst->m_idle[t].f_idle = (f_Notify_idle) callback;
                inst->m_idle[t].d_idle = data;
        } else if ( "notify_resize" == signal ) {
                inst->m_resize[t].f_resize = (f_Notify_resize) callback;
                inst->m_resize[t].d_resize = data;
        } else {
                log_mild_err_dbg ( "no such signal as: %s", signal.c_str () );
                return ;
        }
}

void RenderRegionActiveX::dispatch ( void )
{
        RenderRegionInt *inst = this->pimpl;
        x3d::thr_trap_on_task ( &inst->m_block_driver );

        int t;
        t = (inst->m_iswap + 1) & 1;
        {
                RenderRegionInt::Idle *t_idle = &inst->m_idle[t];
                if ( t_idle->f_idle )
                        t_idle->f_idle ( t_idle->is_idle, inst->m_handle, t_idle->d_idle );
        }
        {
                RenderRegionInt::Resize *t_resize = &inst->m_resize[t];
                if ( t_resize->f_resize || t_resize->is_resized ) {
                        t_resize->f_resize ( t_resize->width, t_resize->height,
                                             t_resize->is_fullscreen,
                                             inst->m_handle,
                                             t_resize->d_resize );
                        t_resize->is_resized = false;
                }
        }

        x3d::thr_untrap_task ( &inst->m_block_driver );
}

void RenderRegionActiveX::update ( void )
{
        RenderRegionInt *inst = this->pimpl;

        /* atomically swap the state buffer */
        x3d::thr_trap_on_task ( &inst->m_block_driver );
        inst->m_iswap ++;
        x3d::thr_untrap_task ( &inst->m_block_driver );

        KernelEnvironment* state = get_state_buffer ();

        struct x3d::renderer* renderer =
                (struct x3d::renderer*) state->use ( c_Renderer );
        if ( renderer != inst->m_rend )
                inst->m_rend = renderer;

        if ( !inst->m_is_idle && inst->m_rend != nullptr )
                x3d::renderer_commit ( inst->m_rend );

        if ( inst->m_is_resized )
                inst->m_is_resized = false;
}

void RenderRegionActiveX::load ( struct serializer *s )
{
}

void RenderRegionActiveX::save ( struct serializer *s )
{
}
