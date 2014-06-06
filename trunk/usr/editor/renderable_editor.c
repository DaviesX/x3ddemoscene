/* renderable_editor.c: implementation of the object renderable control */
#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include "gui.h"
#include "renderable_editor.h"


struct renderable_editor {
        GtkWidget *rda_prop;
};

static struct renderable_editor g_rend_edit;


bool renderable_editor_load ( char *glade_path )
{
        memset ( &g_rend_edit, 0, sizeof g_rend_edit );

        char file[256];
        strcpy ( file, glade_path );
        strcat ( file, "renderable_property_widget.glade" );
        GtkBuilder *builder = builder_load ( file );
        if ( builder == nullptr ) {
                return false;
        }

        g_rend_edit.rda_prop =
                (GtkWidget *) gtk_builder_get_object ( builder, "notebook" );
        if ( !g_rend_edit.rda_prop ) {
                log_severe_err_dbg ( "cannot retrieve renderable property widget" );
                return false;
        }
        builder_all_set ( builder );
        return true;
}

void renderable_editor_set ( int state )
{
}
