/* editrenderregion.cpp: render region activex implementation */
#include <usr/usr_x3d.hpp>
#include <usr/usr_projectionprobe.hpp>
#include <usr/usr_editor.hpp>


namespace x3d
{
namespace usr
{

class RenderRegionActiveX::RenderRegionInt
{
public:
        RenderRegionInt(OutputMethod method, void *handle, int x, int y, int w, int h);
        ~RenderRegionInt();

        struct {        // Idle
                bool            m_is_idle;
                f_Notify_Idle   m_idle_signal;
                void*           m_idle_signal_data;
        };

        struct {        // Resize
                int                     m_width;
                int                     m_height;
                bool                    m_is_resized;
                bool                    m_is_fullscreen;
        };


        struct {        // ViewControl
                void*                   m_handle;
                int                     m_method;
                int                     m_cursor_x0;
                int                     m_cursor_y0;
                int                     m_cursor_x1;
                int                     m_cursor_y1;
                int                     m_delta;
                struct point3d          m_viewpoint;
                ViewMode                m_viewmode;
                PerspectiveProbe*       m_persp;
                OrthogonalProbe*        m_orth;
                ProjectionProbe*        m_probe;
                const float             c_SensitivityFactor = 0.8f;
        };

        // optics properties are default to these values
        const enum ColorMode    c_ColorMode     = Color32Mode;
};

RenderRegionActiveX::RenderRegionInt::RenderRegionInt(OutputMethod method, void *handle, int x, int y, int w, int h)
{
        m_handle        = handle;
        m_is_idle       = true;
        m_is_resized    = false;
        m_method        = method;
        set_point3d(0.0f, 0.0f, 0.0f, &m_viewpoint);
        m_persp         = new PerspectiveProbe(method, handle, w, h, c_ColorMode);
/* @fixme (davis#9#): <RenderRegionActiveX> orthogonal probe not supported yet */
        m_orth          = new OrthogonalProbe(method, handle, w, h, c_ColorMode);;

        m_is_idle               = true;
        m_idle_signal           = nullptr;
        m_idle_signal_data      = nullptr;

        m_width                 = w;
        m_height                = h;
        m_is_resized            = false;
        m_is_fullscreen         = false;

        m_persp->toggle_fullscreen(false);
        m_probe                 = m_persp;
}

RenderRegionActiveX::RenderRegionInt::~RenderRegionInt()
{
        delete m_persp;
        delete m_orth;
        zero_obj(this);
}

RenderRegionActiveX::RenderRegionActiveX(string name, OutputMethod method, void *handle, int x, int y, int w, int h) :
        EditorActiveX(name, sizeof(RenderRegionActiveX), EDIT_ACTIVEX_RENDER_REGION)
{
        pimpl = new RenderRegionInt(method, handle, x, y, w, h);
}

RenderRegionActiveX::~RenderRegionActiveX()
{
        delete pimpl;
}

void RenderRegionActiveX::on_adding()
{
        KernelEnvironment* state = get_state_buffer ();
        state->declare(c_RenderRegion, this);
}

void RenderRegionActiveX::set_idle_state(bool is_idle)
{
        wait_for_update();
        pimpl->m_is_idle = is_idle;
        unwait();
}

void RenderRegionActiveX::resize(int x, int y, int w, int h, bool toggle_fullscreen)
{
        wait_for_update();
        pimpl->m_is_resized             = true;
        pimpl->m_is_fullscreen          = toggle_fullscreen;
        pimpl->m_width                  = w;
        pimpl->m_height                 = h;
        unwait();
}

void RenderRegionActiveX::bind_callback(string signal, f_Generic callback, void* data)
{
        wait_for_update();
        if ("notify_idle" == signal) {
                pimpl->m_idle_signal            = (f_Notify_Idle) callback;
                pimpl->m_idle_signal_data       = data;
        } else {
                log_mild_err_dbg("no such signal as: %s", signal.c_str());
        }
        unwait();
}

ProjectionProbe* RenderRegionActiveX::get_probe() const
{
        return pimpl->m_probe;
}

void RenderRegionActiveX::moveto(int x, int y)
{
        wait_for_update();
        pimpl->m_cursor_x1 = x;
        pimpl->m_cursor_y1 = y;
        unwait();
}

void RenderRegionActiveX::magnify(int delta)
{
        wait_for_update();
        pimpl->m_delta = delta;
        unwait();
}

void RenderRegionActiveX::set_view_mode(ViewMode mode)
{
        wait_for_update();
        switch(mode) {
        case PerspectiveMode:
                pimpl->m_probe = pimpl->m_persp;
                break;
        case OrthogonalMode:
                pimpl->m_probe = pimpl->m_orth;
                break;
        case QuadOrthogonalViewMode:
                log_mild_err_dbg("Quad orthogonal view mode is not supported yet");
                break;
        }
        unwait();
}

void RenderRegionActiveX::dispatch()
{
        wait_for_update();
        if (pimpl->m_idle_signal) {
                pimpl->m_idle_signal(pimpl->m_is_idle, this, pimpl->m_idle_signal_data);
        }
        unwait();
}

void RenderRegionActiveX::update()
{
        wait_for_update();
        KernelEnvironment* state = get_state_buffer();
        // see if there is anything to be processed by the kernel
        WorldDataActiveX* worlddata = (WorldDataActiveX*) state->use(c_WorldData);
        if (worlddata->get_world().get_render_aggregate()->get_instance_count() == 0) {
                pimpl->m_is_idle = true;
        } else {
                pimpl->m_is_idle = false;
        }
        // see if there is anything to commit to the screen
        RenderConfigActiveX* config = (RenderConfigActiveX*) state->use(c_RenderConfig);
        if (!pimpl->m_is_idle && config != nullptr) {
                config->get_renderer()->commit();
        }
        // any request to change the state of the screen
        if (pimpl->m_is_resized) {
                pimpl->m_persp->set_output_format(pimpl->m_width, pimpl->m_height, pimpl->c_ColorMode);
                pimpl->m_persp->toggle_fullscreen(pimpl->m_is_fullscreen);
                pimpl->m_is_resized = false;
        }
        // set position and direction
        int delta_x = pimpl->m_cursor_x1 - pimpl->m_cursor_x0;
        int delta_y = pimpl->m_cursor_y1 - pimpl->m_cursor_y0;
        float x_rot = (float) delta_x/pimpl->m_width*M_PI;
        float y_rot = (float) delta_y/pimpl->m_height*M_PI;
        pimpl->m_probe->rotate_relative(x_rot, y_rot, 0.0f);
        pimpl->m_probe->move_relative(pimpl->m_delta*pimpl->c_SensitivityFactor);
        pimpl->m_cursor_x0 = pimpl->m_cursor_x1;
        pimpl->m_cursor_y0 = pimpl->m_cursor_y1;
        unwait();
}

void RenderRegionActiveX::load(struct serializer *s)
{
}

void RenderRegionActiveX::save(struct serializer *s)
{
}

}// namespace usr

}// namespace x3d
