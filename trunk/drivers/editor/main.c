#if 0
#include <x3d/common.h>
#include <x3d/editor.h>
#include <editor/gui.h>

// #define DEBUG_MODE

int main ( int argc, char *argv[] )
{
#ifndef DEBUG_MODE
        gtk_init ( &argc, &argv );

        /* Load in the window frame */
        if ( !(load_main_window ( &argc, &argv, 0 )) ) {
                return -1;
        }
        rp_init ();
        gtk_main ();
        rp_free ();
        return 0;
#else
        debug_run_x3d ( "resource_piler_test" );
        return 1;
#endif
}

#endif
