#include <stdio.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_kernel.hpp>
#include <usr/usr_editor.hpp>

using namespace x3d;
using namespace x3d::usr;

extern "C" int kernel_main ( int argc, char *argv[] )
{
        x3d::kernel_add_param ( (char*) "--edit-mode", (char*) "" );

        Kernel *kernel = new Kernel ();
        Editor *edit = new Editor ();
        KernelEditor *kern_edit = new KernelEditor ( edit );
        kern_edit->register_gui_frontend ( GUI_FONTEND_GTK, nullptr );

        kernel->register_proxy ( kern_edit );
        kernel->run ();

        delete kernel;
        delete edit;
        delete kern_edit;
        return 0;
}
