/* 
 * File:   editrunner.cpp
 * Author: Wen, Chifeng <https://sourceforge.net/u/daviesx/profile/>
 *
 * Created on October 22, 2015, 8:34 AM
 */
#include <usr/usr_editor.hpp>

namespace x3d
{
namespace usr
{        
class RenderFrameActiveX::RenderFrameInt
{
public:
        RenderFrameInt();
        ~RenderFrameInt();
        
        bool                            m_torun = false;
        
        std::list<std::string>          m_errors;
        f_Notify_Error                  f_notify_error = nullptr;
        void*                           f_notify_error_data = nullptr;
        
        f_Notify_In_Progress            f_notify_in_progress = nullptr;
        void*                           f_notify_in_progress_data = nullptr;
        bool                            m_in_progress = false;
        
        Renderer                        m_renderer;
        uuid_t                          m_rendid = -1;
        f_Notify_Finish                 f_notify_finish = nullptr;
        void*                           f_notify_finish_data = nullptr;
        bool                            m_has_finished = false;
};

RenderFrameActiveX::RenderFrameInt::RenderFrameInt()
{
}

RenderFrameActiveX::RenderFrameInt::~RenderFrameInt()
{
}

RenderFrameActiveX::RenderFrameActiveX(string name) : 
        EditorBackendActiveX(name, sizeof(*this), EditActiveXRenderFrame)
{
        pimpl = new RenderFrameActiveX::RenderFrameInt();
}


RenderFrameActiveX::~RenderFrameActiveX()
{
        delete pimpl;
}

void RenderFrameActiveX::on_adding()
{
        KernelEnvironment* state = get_state_buffer ();
        state->declare(c_RenderFrame, this);
}

void RenderFrameActiveX::preupdate()
{
}

void RenderFrameActiveX::update()
{
        wait_for_update();
        
        WorldDataActiveX* worldax       = static_cast<WorldDataActiveX*>(get_state_buffer()->use(c_WorldData));
        RenderRegionActiveX* regionax   = static_cast<RenderRegionActiveX*>(get_state_buffer()->use(c_RenderRegion));
        if (pimpl->m_torun) {
                // shutdown other tasks
                regionax->request_idle_state(true);
                
                // configure the renderer
                RenderConfigActiveX* config = static_cast<RenderConfigActiveX*>(get_state_buffer()->use(c_RenderConfig));
                RenderTree* tree = config->get_render_tree(RenderConfigActiveX::RenderTreeStatic);
                pimpl->m_renderer.update(tree);
                
                // this activex only run the task for one time
                pimpl->m_rendid = worldax->get_world().bind_render_processor(&pimpl->m_renderer);
                pimpl->m_in_progress = true;
                pimpl->m_torun = false;
        } else {
                // detach renderer and resume other tasks
                if (pimpl->m_in_progress) {
                        regionax->request_idle_state(false);
                        worldax->get_world().detach_render_processor(pimpl->m_rendid);
                        pimpl->m_in_progress = false;
                        pimpl->m_has_finished = true;
                }
        }
        
        unwait();
}

void RenderFrameActiveX::dispatch()
{
        wait_for_update();
        if (pimpl->m_has_finished && pimpl->f_notify_finish) {
                pimpl->f_notify_finish("", *this, pimpl->f_notify_finish_data);
                pimpl->m_has_finished = false;
        }
        if (pimpl->m_in_progress && pimpl->f_notify_in_progress) {
                pimpl->f_notify_in_progress("", *this, pimpl->f_notify_in_progress_data);
        }
        if (pimpl->f_notify_error) {
                while (!pimpl->m_errors.empty()) {
                        pimpl->f_notify_error(pimpl->m_errors.back(), *this, pimpl->f_notify_error_data);
                        pimpl->m_errors.pop_back();
                }
        } else {
                pimpl->m_errors.clear();
        }
        unwait();
}

void RenderFrameActiveX::load(struct serializer* s)
{
}

void RenderFrameActiveX::save(struct serializer* s)
{
}

void RenderFrameActiveX::bind_callback(string signal, f_Generic callback, void *data)
{
        if (signal == "notify_error") {
                pimpl->f_notify_error = (f_Notify_Error) callback;
                pimpl->f_notify_error_data = data;
        } else if (signal == "notify_finish") {
                pimpl->f_notify_finish = (f_Notify_Finish) callback;
                pimpl->f_notify_finish_data = data;
        } else if (signal == "notify_in_progress") {
                pimpl->f_notify_in_progress = (f_Notify_In_Progress) callback;
                pimpl->f_notify_in_progress_data = data;
        } else {
                log_mild_err("No such signal as: %s", signal.c_str());
        }
}

/**@todo #davis#5# <run_frame_renderer> no way to stop the rendering process */
bool RenderFrameActiveX::run_frame_renderer(bool to_run)
{
        wait_for_update();
        pimpl->m_torun = to_run;
        unwait();
        return true;
}

} // namespace usr

} // namespace x3d