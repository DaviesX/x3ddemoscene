/* main.c: Unit tests to all shader library functions go here */
#include "main.h"
#include <renderer/shader.h>


void dbg_shader_add_all ( void )
{
        static bool first_time = true;
        if ( first_time ) {
                init_shader_library ();
        }
}
