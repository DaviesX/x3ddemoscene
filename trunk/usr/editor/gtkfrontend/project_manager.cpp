#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"


namespace x3d
{
namespace usr
{

class ProjectManager::ProjectManagerInt
{
public:
        ProjectManagerInt(EditorGtkFrontend* frontend);
        ~ProjectManagerInt();
public:
        const string            c_WorldDataString;
        EditorGtkFrontend*      m_frontend;
};

ProjectManager::ProjectManagerInt::ProjectManagerInt(EditorGtkFrontend* frontend) :
        c_WorldDataString("project-manager-data"),
        m_frontend(frontend)
{
}

ProjectManager::ProjectManagerInt::~ProjectManagerInt()
{
        m_frontend = nullptr;
}

ProjectManager::ProjectManager(EditorGtkFrontend* frontend)
{
        pimpl = new ProjectManager::ProjectManagerInt(frontend);
}

ProjectManager::~ProjectManager()
{
        delete pimpl;
}

bool ProjectManager::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;

        if (visible) {
                frontend->get_core_editor()->add_activex(new WorldDataActiveX(pimpl->c_WorldDataString));
        }
        return true;
}

}// namespace usr

}// namespace x3d
