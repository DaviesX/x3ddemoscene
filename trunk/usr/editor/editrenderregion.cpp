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

        x3d::build_irectangle_2d ( x, y, x + w, y + h, &inst->m_rect );
        inst->m_handle          = handle;
        inst->m_rend            = nullptr;
        inst->m_is_idle         = true;
        inst->m_is_resized      = false;

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
        this->pimpl->m_is_idle = is_idle;
        this->pimpl->m_idle[on_front_buf()].is_idle = is_idle;
}

void RenderRegionActiveX::set_renderer ( struct renderer *renderer )
{
        this->pimpl->m_rend = renderer;
}

void RenderRegionActiveX::resize ( int x, int y, int w, int h, bool toggle_fullscreen )
{
        pimpl->m_resize[on_front_buf()].is_resized      = true;
        pimpl->m_resize[on_front_buf()].width           = w;
        pimpl->m_resize[on_front_buf()].height          = h;
        pimpl->m_resize[on_front_buf()].is_fullscreen   = toggle_fullscreen;
        pimpl->m_is_resized                             = true;
        x3d::build_irectangle_2d ( x, y, x + w, y + h, &pimpl->m_rect );
}

void RenderRegionActiveX::bind_callback ( string signal, f_Generic callback, void *data )
{
        if ( "notify_idle" == signal ) {
                pimpl->m_idle[on_front_buf()].f_idle = (f_Notify_idle) callback;
                pimpl->m_idle[on_front_buf()].d_idle = data;
        } else if ( "notify_resize" == signal ) {
                pimpl->m_resize[on_front_buf()].f_resize = (f_Notify_resize) callback;
                pimpl->m_resize[on_front_buf()].d_resize = data;
        } else {
                log_mild_err_dbg ( "no such signal as: %s", signal.c_str () );
                return ;
        }
}

const void* RenderRegionActiveX::get_handle ( void ) const
{
        return this->pimpl->m_handle;
}

void RenderRegionActiveX::dispatch ( void )
{
        wait_for_update ();
        {
                RenderRegionInt::Idle *t_idle = &pimpl->m_idle[on_back_buf()];
                if ( t_idle->f_idle )
                        t_idle->f_idle ( t_idle->is_idle, this, t_idle->d_idle );
        }
        {
                RenderRegionInt::Resize *t_resize = &pimpl->m_resize[on_back_buf()];
                if ( t_resize->f_resize || t_resize->is_resized ) {
                        t_resize->f_resize ( t_resize->is_fullscreen,
                                             t_resize->width, t_resize->height,
                                             this, t_resize->d_resize );
                        t_resize->is_resized = false;
                }
        }
        unwait ();
}

void RenderRegionActiveX::update ( void )
{
        /* atomically swap the state buffer */
        wait_for_update ();
        swap_buf ();
        unwait ();

        KernelEnvironment* state = get_state_buffer ();

        struct x3d::renderer* renderer =
                (struct x3d::renderer*) state->use ( c_Renderer );
        if ( renderer != pimpl->m_rend )
                pimpl->m_rend = renderer;

        if ( !pimpl->m_is_idle && pimpl->m_rend != nullptr )
                x3d::renderer_commit ( pimpl->m_rend );

        if ( pimpl->m_is_resized )
                pimpl->m_is_resized = false;
}

void RenderRegionActiveX::load ( struct serializer *s )
{
}

void RenderRegionActiveX::save ( struct serializer *s )
{
}
