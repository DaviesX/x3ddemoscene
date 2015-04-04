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
        stop_gtk_main();
        await_gtk_main();

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

enum
{
        COL_NAME,
        COL_AGE,
        NUM_COLS
};


static GtkTreeModel* create_and_fill_model()
{
        GtkTreeStore  *store;
        GtkTreeIter    iter;

        store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

        /* Append a row and fill in some data */
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter,
                           COL_NAME, "Heinz El-Mann",
                           COL_AGE, 51,
                           -1);
        GtkTreeIter   parent = iter;

        /* append another row and fill in some data */
        gtk_tree_store_insert(store, &iter, &parent, -1);
        gtk_tree_store_set(store, &iter,
                           COL_NAME, "Jane Doe",
                           COL_AGE, 23,
                           -1);

        /* ... and a third row */
        gtk_tree_store_append (store, &iter, &parent);
        gtk_tree_store_set(store, &iter,
                           COL_NAME, "Joe Bungop",
                           COL_AGE, 91,
                           -1);

        return GTK_TREE_MODEL(store);
}

static void fill_in_tree_view_and_model(GtkTreeView* view)
{
        GtkCellRenderer     *renderer;
        GtkTreeModel        *model;

        /* --- Column #1 --- */
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                    -1,
                                                   "Name",
                                                   renderer,
                                                   "text", COL_NAME,
                                                   NULL);

        /* --- Column #2 --- */
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                    -1,
                                                    "Age",
                                                    renderer,
                                                    "text", COL_AGE,
                                                    NULL);
        model = create_and_fill_model();
        gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

        /* The tree view has acquired its own reference to the
         *  model, so we can drop ours. That way the model will
         *  be freed automatically when the tree view is destroyed */
        g_object_unref(model);
}

bool ProjectManager::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;
        if (frontend->get_editor_mode() == EditorGtkFrontend::DemoMode)
                return true;
        GtkBuilder* builder = frontend->get_gtk_builder();
        if (!builder)
                return false;
        GtkTreeView* tree_view = (GtkTreeView*) gtk_builder_get_object(builder, "TV-ProjectManager");
        if (!tree_view) {
                log_severe_err_dbg("cannot obtain TV-ProjectManager widget");
                return false;
        }
        fill_in_tree_view_and_model(tree_view);

        if (visible) {
                frontend->get_core_editor()->add_activex(new WorldDataActiveX(pimpl->c_WorldDataString));
        }
        return true;
}

}// namespace usr

}// namespace x3d
