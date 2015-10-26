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

        EditorGtkFrontend*      m_frontend;
        
        GtkLabel*               m_project_label;
        
        GtkTreeView*            m_project_tree;
        GtkTreeStore*           m_project_tree_data;
        
        GtkTreeIter             m_rda_iter;
        GtkTreeIter             m_rda_inst_iter;
};

ProjectManager::ProjectManagerInt::ProjectManagerInt(EditorGtkFrontend* frontend) :
        m_frontend(frontend)
{
}

ProjectManager::ProjectManagerInt::~ProjectManagerInt()
{
        stop_gtk_main();
        await_gtk_main();
        
        g_object_unref(m_project_tree_data);
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

enum ColumnStructure
{
        COL_NAME,
        NUM_COLS
};

bool ProjectManager::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;
        if (frontend->get_editor_mode() == EditorGtkFrontend::DemoMode)
                return true;
        GtkBuilder* builder = frontend->get_gtk_builder();
        if (!builder)
                return false;
        
        
        // Pre-fill the tree with the categories
        {
                pimpl->m_project_tree = (GtkTreeView*) gtk_builder_get_object(builder, "TV-ProjectManager");
                if (!pimpl->m_project_tree) {
                        log_severe_err_dbg("cannot obtain TV-ProjectManager widget");
                        return false;
                }
                GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
                gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pimpl->m_project_tree),
                                                            -1, "X3d", renderer, "text", COL_NAME, NULL);
                gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(pimpl->m_project_tree), false);
                // build up data
                pimpl->m_project_tree_data = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);
                GtkTreeIter    iter;
                gtk_tree_store_append(pimpl->m_project_tree_data, &iter, NULL);
                gtk_tree_store_set(pimpl->m_project_tree_data, &iter, COL_NAME, "Renderable", -1);
                pimpl->m_rda_iter = iter;
                gtk_tree_store_append(pimpl->m_project_tree_data, &iter, NULL);
                gtk_tree_store_set(pimpl->m_project_tree_data, &iter, COL_NAME, "Renderable Instance", -1);
                pimpl->m_rda_inst_iter = iter;

                // writing test data to renderable node
                iter = pimpl->m_rda_iter;
                gtk_tree_store_insert(pimpl->m_project_tree_data, &iter, &iter, -1);
                gtk_tree_store_set(pimpl->m_project_tree_data, &iter, COL_NAME, "r1", -1);
                // writing test data to renderable instance node
                iter = pimpl->m_rda_inst_iter;
                gtk_tree_store_insert(pimpl->m_project_tree_data, &iter, &iter, -1);
                gtk_tree_store_set(pimpl->m_project_tree_data, &iter, COL_NAME, "ri1", -1);

                gtk_tree_view_set_model(GTK_TREE_VIEW(pimpl->m_project_tree), 
                                        GTK_TREE_MODEL(pimpl->m_project_tree_data));       
        }
        {
                pimpl->m_project_label = (GtkLabel*) gtk_builder_get_object(builder, "LB-ProjectName");
                if (!pimpl->m_project_label) {
                        log_severe_err_dbg("cannot obtain LB-ProjectName widget");
                        return false;
                }
                gtk_label_set_text(GTK_LABEL(pimpl->m_project_label), "<Invalid Project>");
        }

        if (visible) {
                WorldDataActiveX* worldax = new WorldDataActiveX(gtkactivex::c_BackendWorldData);
                if (!frontend->get_backend_editor()->add_activex(worldax)) {
                        delete worldax;
                } else {
                        gtk_label_set_text(GTK_LABEL(pimpl->m_project_label), 
                                           worldax->get_data_set_name().c_str());
                        MainEditor* mainedit = pimpl->m_frontend->get_main_editor();
                        std::string title = worldax->get_data_set_name() + " - " + worldax->get_storage_path();
                        mainedit->set_window_title(title);
                }
        }
        return true;
}

}// namespace usr

}// namespace x3d
