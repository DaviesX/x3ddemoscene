/* renderer_configurator.cpp: gtk implementation of renderer configurator dialog */
#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"
#include "renderer_configurator.hpp"
#include "main_editor.hpp"

namespace x3d
{
namespace usr
{

class RendererConfig::RendererConfigInt
{
public:
        RendererConfigInt(EditorGtkFrontend* frontend);
        ~RendererConfigInt();
public:
        EditorGtkFrontend*      m_frontend;
};


static void render_config_error_callback(string message, RenderConfigActiveX *conf, void *user_data)
{
        EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(user_data);
        frontend->get_main_editor()->send_message_box("X3d Renderer Configurator", message, MainEditor::MessageError);
}

RendererConfig::RendererConfigInt::RendererConfigInt(EditorGtkFrontend* frontend)
{
        m_frontend = frontend;
}

RendererConfig::RendererConfigInt::~RendererConfigInt()
{
        m_frontend = nullptr;
}

RendererConfig::RendererConfig(EditorGtkFrontend* frontend)
{
        pimpl = new RendererConfig::RendererConfigInt(frontend);
}

RendererConfig::~RendererConfig()
{
        delete pimpl;
}

bool RendererConfig::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;
        // load in GUI
        // apply activex
        if (visible) {
                RenderConfigActiveX* config = new RenderConfigActiveX(gtkactivex::c_BackendRendererConfig);
                config->bind_callback("notify_error", (f_Generic) render_config_error_callback, frontend);
                if (!frontend->get_backend_editor()->add_activex(config)) {
                        delete config;
                }
        }
        return true;
}

}// namespace usr

}// namespace x3d
