#include <gtk/gtk.h>
#include <logout.h>
#include <x3d/common.h>
#include "gui.h"
#include "splash_screen.h"


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
