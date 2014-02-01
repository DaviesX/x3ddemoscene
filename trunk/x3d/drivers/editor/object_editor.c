/* object_editor.c: implementation of the object editor control */
#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include "gui.h"
#include "object_editor.h"


struct object_editor {
	GtkWidget *window;
};

static struct object_editor g_obj_edit;


bool object_editor_load ( void )
{
	GtkBuilder *builder = g_comm_data.builder;
	g_obj_edit.window =
		GTK_WIDGET(gtk_builder_get_object ( builder, "object-editor" ));
	if ( !g_obj_edit.window ) {
		log_severe_err_dbg ( "cannot retrieve object-editor window" );
		return false;
	}
	return true;
}

void object_editor_set ( int state )
{
}
