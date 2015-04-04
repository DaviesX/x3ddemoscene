/* entity_editor.cpp: implementation of the entity editor control */
#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{

class EntityEditor::EntityEditorInt {
public:
        EntityEditorInt(EditorGtkFrontend* frontend);
        ~EntityEditorInt();

        EditorGtkFrontend*      m_frontend;

        GtkWidget*              m_hier_tree;
        GtkWidget*              m_prop_notebook;
        GtkListStore*           m_stored_obj;
};

EntityEditor::EntityEditorInt::EntityEditorInt(EditorGtkFrontend* frontend)
{
        m_frontend = frontend;
}

EntityEditor::EntityEditorInt::~EntityEditorInt()
{
        stop_gtk_main();
        await_gtk_main();

        m_frontend = nullptr;
}

EntityEditor::EntityEditor(EditorGtkFrontend* frontend)
{
        pimpl = new EntityEditor::EntityEditorInt(frontend);
}

EntityEditor::~EntityEditor()
{
        delete pimpl;
}

bool EntityEditor::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;

        GtkBuilder* builder = frontend->get_gtk_builder();
        if (builder == nullptr) {
                return false;
        }
        {
                pimpl->m_prop_notebook = (GtkWidget*) gtk_builder_get_object(builder, "entity-notebook");
                if (!pimpl->m_prop_notebook) {
                        log_severe_err_dbg("cannot retrieve entity property notebook widget");
                        return false;
                }
        }
        {
                pimpl->m_hier_tree = (GtkWidget*) gtk_builder_get_object(builder, "entity-hierachy-tree");
                if (!pimpl->m_hier_tree) {
                        log_severe_err_dbg("cannot retrieve entity hierachy tree widget");
                        return false;
                }
                // seems like list store object is not importable
                /* pimpl->m_stored_obj = (GtkListStore*) gtk_builder_get_object(builder, "tree-list-store");
                if (pimpl->m_stored_obj) {
                        log_severe_err_dbg("cannot retrieve entity tree list store");
                        return false;
                } */
        }
        if (visible) {
                // perform injection to the main editor
                // MainEditor* editor = frontend->get_main_editor();
        }
        return true;
}

}// namespace usr

}// namespace x3d
