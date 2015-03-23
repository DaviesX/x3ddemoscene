/* entity_editor.cpp: implementation of the entity editor control */
#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"
#include "entity_editor.hpp"

namespace x3d
{
namespace usr
{

struct entity_editor {
        GtkWidget *hier_tree;
        GtkWidget *prop_notebook;
        GtkListStore *stored_obj;
};

static struct entity_editor g_ent_edit;


bool entity_editor_load ( char *glade_path )
{
        memset ( &g_ent_edit, 0, sizeof g_ent_edit );

        char file[256];
        {
                strcpy ( file, glade_path );
                strcat ( file, "entity_property_widget.glade" );
                GtkBuilder* builder = builder_load(file);
                if ( builder == nullptr ) {
                        return false;
                }

                g_ent_edit.prop_notebook =
                        (GtkWidget *) gtk_builder_get_object ( builder, "notebook" );
                if ( !g_ent_edit.prop_notebook ) {
                        log_severe_err_dbg ( "cannot retrieve entity property notebook widget" );
                        return false;
                }
                builder_all_set ( builder );
        }
        {
                strcpy ( file, glade_path );
                strcat ( file, "entity_tree_widget.glade" );
                GtkBuilder *builder = builder_load ( file );
                if ( builder == nullptr ) {
                        return false;
                }

                g_ent_edit.hier_tree =
                        (GtkWidget *) gtk_builder_get_object ( builder, "hierachy-tree" );
                if ( !g_ent_edit.hier_tree ) {
                        log_severe_err_dbg ( "cannot retrieve entity hierachy tree widget" );
                        return false;
                }
                g_ent_edit.stored_obj =
                        (GtkListStore *) gtk_builder_get_object ( builder, "tree-list-store" );
                if ( !g_ent_edit.stored_obj ) {
                        log_severe_err_dbg ( "cannot retrieve entity tree list store" );
                        return false;
                }
                builder_all_set ( builder );
        }
        return true;
}

}// namespace usr

}// namespace x3d
