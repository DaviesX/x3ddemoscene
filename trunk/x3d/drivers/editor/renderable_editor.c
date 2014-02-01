/* renderable_editor.c: implementation of the object renderable control */
#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include "gui.h"
#include "renderable_editor.h"


struct renderable_editor {
	GtkWidget *window;
};

static struct renderable_editor g_rend_edit;


bool renderable_editor_load ( void )
{
	GtkBuilder *builder = g_comm_data.builder;
	g_rend_edit.window =
		GTK_WIDGET(gtk_builder_get_object ( builder, "renderable-editor" ));
	if ( !g_rend_edit.window ) {
		log_severe_err_dbg ( "cannot retrieve renderable-editor window" );
		return false;
	}
	return true;
}

void renderable_editor_set ( int state )
{
}
