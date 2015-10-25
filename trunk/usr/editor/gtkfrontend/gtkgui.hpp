#ifndef GTKGUI_HPP_INCLUDED
#define GTKGUI_HPP_INCLUDED

#include <gtk/gtk.h>
#include <usr/usr_editorfrontend.hpp>
#include "splash_screen.hpp"
#include "main_editor.hpp"
#include "main_editor_menu.hpp"
#include "project_manager.hpp"
#include "entity_editor.hpp"
#include "renderer_configurator.hpp"
#include "renderable_editor.hpp"
#include "dynamics_editor.hpp"

namespace x3d
{
namespace usr
{

/*
 * functions' declaration
 */
string file_chooser_open(string default_dir);
string file_chooser_save(string default_dir);
void message_box_info(string title, string message, GtkWindow* parent);
void message_box_warning(string title, string message, GtkWindow* parent);
void message_box_error(string title, string message, GtkWindow* parent);
bool message_box_question(string title, string message, GtkWindow* parent);

void widget_get_size(GtkWidget* parent, GtkWidget* widget,
                       int* x, int* y, int* width, int* height);
void run_gtk_main(void);
void stop_gtk_main(void);
void await_gtk_main(void);

}// namespace usr

}// namespace x3d

#endif // GTKGUI_HPP_INCLUDED
