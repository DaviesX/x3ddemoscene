/* renderer_configurator.cpp: gtk implementation of renderer configurator dialog */
#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"
#include "renderer_configurator.hpp"
#include "main_editor.hpp"

using namespace x3d;
using namespace x3d::usr;


static const string cRenderConfig = "gtk-main-editor-render-config";
static void render_config_error_callback ( string message, RenderConfigActiveX *conf, void *data );


static void render_config_error_callback ( string message, RenderConfigActiveX *conf, void *data )
{
        message_box_error ( cRenderConfig, message );
}

bool EditorGtkFrontend::render_config_load ( void )
{
        RenderConfigActiveX* rcax = new RenderConfigActiveX ( cRenderConfig );
        rcax->bind_callback ( "notify_error", (f_Generic) render_config_error_callback, nullptr );
        this->m_editor->add_activex ( rcax );
        return true;
}

bool EditorGtkFrontend::render_config_show ( bool is_visible )
{
        return true;
}

bool EditorGtkFrontend::render_config_shut ( void )
{
        return true;
}
