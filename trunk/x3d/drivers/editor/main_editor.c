/* main_editor.c: implementation of the main editor control */
#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include <x3d/editor.h>
#include "gui.h"
#include "main_editor.h"


struct main_editor {
	GtkWidget *curr_window;
	GtkWidget *curr_draw_area;
	GtkWidget *edit_window;
	GtkWidget *pure_window;
	GtkWidget *pure_draw_area;
	GtkWidget *draw_area;
	GtkWidget *element_tree_view;
	GtkWidget *new_assist_dialog;

	int logo_draw_signal;
};

static struct main_editor g_main_edit;

static void display_logo_image ( GtkWidget *draw_area, bool to_display );
static void switch_to_edit ( void );
static void switch_to_pure ( void );


static void switch_to_edit ( void )
{
}

static void switch_to_pure ( void )
{
}

static gboolean display_logo_callback ( GtkWidget *draw_region,
					cairo_t *cairo, gpointer data )
{
	gdk_cairo_set_source_pixbuf ( cairo,
				      g_comm_data.logo_pix_buf,
				      0, 0 );
	cairo_paint ( cairo );
	cairo_fill ( cairo );
	return 0;
}

static void display_logo_image ( GtkWidget *draw_area, bool to_display )
{
	if ( to_display ) {
		g_main_edit.logo_draw_signal =
			g_signal_connect ( draw_area, "draw",
					   G_CALLBACK ( display_logo_callback ), nullptr );
	} else {
		g_signal_handler_disconnect ( draw_area,
					      g_main_edit.logo_draw_signal );
	}
}

bool main_editor_load ( void )
{
	memset ( &g_main_edit, 0, sizeof g_main_edit );
	GtkBuilder *builder = g_comm_data.builder;
	g_main_edit.edit_window =
		GTK_WIDGET(gtk_builder_get_object ( builder, "main-editor" ));
	if ( !g_main_edit.edit_window ) {
		log_severe_err_dbg ( "cannot retrieve main-editor window" );
		return false;
	}
	g_main_edit.draw_area =
		GTK_WIDGET(gtk_builder_get_object ( builder, "draw-area" ));
	if ( !g_main_edit.draw_area ) {
		log_severe_err_dbg ( "cannot retrieve draw-area widget" );
		return false;
	}
	g_main_edit.pure_window =
		GTK_WIDGET(gtk_builder_get_object ( builder, "pure-window" ));
	if ( !g_main_edit.pure_window ) {
		log_severe_err_dbg ( "cannot retrieve pure-window" );
		return false;
	}
	g_main_edit.pure_draw_area =
		GTK_WIDGET(gtk_builder_get_object ( builder, "pure-draw-area" ));
	if ( !g_main_edit.pure_draw_area ) {
		log_severe_err_dbg ( "cannot retrieve pure-draw-area widget" );
		return false;
	}
	g_main_edit.element_tree_view =
		GTK_WIDGET(gtk_builder_get_object ( builder, "element-tree-view" ));
	if ( !g_main_edit.element_tree_view ) {
		log_severe_err_dbg ( "cannot retrieve element-tree-view widget" );
		return false;
	}
	g_main_edit.new_assist_dialog =
		GTK_WIDGET(gtk_builder_get_object ( builder, "new-assist-dialog" ));
	if ( !g_main_edit.new_assist_dialog ) {
		log_severe_err_dbg ( "cannot retrieve new-assist-dialog" );
		return false;
	}
	/* set current control window */
	if ( g_comm_data.mode == EDIT_RUN_EDITOR ) {
		g_main_edit.curr_window = g_main_edit.edit_window;
		g_main_edit.curr_draw_area = g_main_edit.draw_area;
	} else {
		g_main_edit.curr_window = g_main_edit.pure_window;
		g_main_edit.curr_draw_area = g_main_edit.pure_draw_area;
	}
	/* change background color to black */
	GdkColor color;
	color.red   = 0X0;
	color.blue  = 0X0;
	color.green = 0X0;
	gtk_widget_modify_bg ( g_main_edit.curr_draw_area, GTK_STATE_NORMAL, &color );
	/* show window */
	gtk_widget_show_all ( g_main_edit.curr_window );
	/* upload render region */
	int x, y;
	int width, height;
	widget_get_size ( g_main_edit.curr_window, g_main_edit.curr_draw_area,
			  &x, &y, &width, &height );
	struct render_region rr;
	rr.type = PLAT_HANDLE_GTK;
	rr.handle = g_main_edit.curr_draw_area;
	rr.rect.x0 = x;
	rr.rect.y0 = y;
	rr.rect.x1 = width;
	rr.rect.y1 = height;
	render_region_add ( &rr );

	display_logo_image ( g_main_edit.curr_draw_area, true );
	return true;
}

void main_editor_set ( int state )
{
}
