/* renderable_editor.c: implementation of the object renderable control */
#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{

class RenderableEditor::RenderableEditorInt
{
public:
        RenderableEditorInt(EditorGtkFrontend* frontend);
        ~RenderableEditorInt();
public:
        EditorGtkFrontend*      m_frontend;
        GtkWidget*              m_property_box;
};

RenderableEditor::RenderableEditorInt::RenderableEditorInt(EditorGtkFrontend* frontend)
{
        m_frontend      = frontend;
        m_property_box  = nullptr;
}

RenderableEditor::RenderableEditorInt::~RenderableEditorInt()
{
        stop_gtk_main();
        await_gtk_main();

        m_frontend      = nullptr;
}

RenderableEditor::RenderableEditor(EditorGtkFrontend* frontend)
{
        pimpl = new RenderableEditor::RenderableEditorInt(frontend);
}

RenderableEditor::~RenderableEditor()
{
        delete pimpl;
}

bool RenderableEditor::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;
        GtkBuilder* builder = frontend->get_gtk_builder();
        if (builder == nullptr) {
                return false;
        }

        pimpl->m_property_box = (GtkWidget*) gtk_builder_get_object(builder, "renderable-notebook");
        if (!pimpl->m_property_box) {
                log_severe_err_dbg ( "cannot retrieve renderable property widget" );
                return false;
        }
        if (visible) {
                // perform injection to the property box
        }
        return true;
}

}// namespace usr

}// namespace x3d
