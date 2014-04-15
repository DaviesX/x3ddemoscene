#include <x3d/common.h>
#include <x3d/init.h>


void kernel_panic ( void )
{
        static int n_recover = 0;
        if ( n_recover > 0 ) {
                puts ( "kernel panic, will now exit" );
                abort ();
        }
}
