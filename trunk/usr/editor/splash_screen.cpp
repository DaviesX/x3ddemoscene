#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorbackend.hpp>
#include "gui.hpp"
#include "splash_screen.hpp"

using namespace x3d;

bool splash_screen_show ( char *glade_path )
{
        char file[256];
        strcpy ( file, glade_path );
        strcat ( file, "splash_screen_window.glade" );
        GtkBuilder *builder = builder_load ( file );
        if ( builder == nullptr ) {
                return false;
        }
        return true;
}


bool splash_screen_shut ( void )
{
        return true;
}
