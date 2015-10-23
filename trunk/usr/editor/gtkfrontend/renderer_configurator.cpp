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

static const string cRenderConfig = "gtk-main-editor-render-config";

static void render_config_error_callback(string message, RenderConfigActiveX *conf, void *data)
{
        message_box_error(cRenderConfig, message);
}

RendererConfig::RendererConfigInt::RendererConfigInt(EditorGtkFrontend* frontend)
{
        m_frontend = frontend;
}

RendererConfig::RendererConfigInt::~RendererConfigInt()
{
        stop_gtk_main();
        await_gtk_main();

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
                RenderConfigActiveX* config = new RenderConfigActiveX(cRenderConfig);
                config->bind_callback("notify_error", (f_Generic) render_config_error_callback, nullptr);
                frontend->get_backend_editor()->add_activex(config);
        } else {
                EditorBackend* edit = frontend->get_backend_editor();
                EditorBackendActiveX* config = edit->find_activex(EditorBackendActiveX::EditActiveXRenderConfig,
                                                           cRenderConfig);
                if (config)
                        edit->remove_activex(EditorBackendActiveX::EditActiveXRenderConfig, cRenderConfig);
        }
        return true;
}

}// namespace usr

}// namespace x3d
