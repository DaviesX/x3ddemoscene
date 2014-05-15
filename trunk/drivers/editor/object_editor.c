/* object_editor.c: implementation of the object editor control */
#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include "gui.h"
#include "object_editor.h"


struct entity_editor {
        GtkWidget *window;
};

static struct entity_editor g_ent_edit;


bool entity_editor_load ( char *glade_path )
{
        memset ( &g_ent_edit, 0, sizeof g_ent_edit );

        char file[256];
        strcpy ( file, glade_path );
        strcat ( file, "entity_editor_widget.glade" );
        GtkBuilder *builder = builder_load ( file );
        if ( builder == nullptr ) {
                return false;
        }

        g_ent_edit.window =
                GTK_WIDGET(gtk_builder_get_object ( builder, "object-editor" ));
        if ( !g_ent_edit.window ) {
                log_severe_err_dbg ( "cannot retrieve object-editor window" );
                return false;
        }
        return true;
}

void entity_editor_set ( int state )
{
}
