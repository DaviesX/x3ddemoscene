#include <usr/usr_editorfrontend.hpp>
#include "project_manager.hpp"


using namespace x3d::usr;

bool EditorGtkFrontend::project_manager_load ( void )
{
        get_core_editor()->add_activex(new WorldDataActiveX("project-manager-data"));
        return true;
}

bool EditorGtkFrontend::project_manager_show ( bool is_visible )
{
        return true;
}

bool EditorGtkFrontend::project_manager_shut ( void )
{
        return true;
}
