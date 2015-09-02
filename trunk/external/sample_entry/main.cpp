#include <stdio.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_kernel.hpp>
#include <usr/usr_editor.hpp>

using namespace x3d;
using namespace x3d::usr;

extern "C" int kernel_main(int argc, char* argv[])
{

        kernel_add_param("--test", "variable_memory_test0");
        kernel_add_param("--edit-mode", "");

        Editor* editor          = new Editor();
        EditorBackend* backend  = new EditorBackend(editor);
        Kernel* kernel          = new Kernel();
        backend->register_gui_frontend(EditorBackend::GUI_FONTEND_GTK, nullptr);
        kernel->register_proxy(backend);
        kernel->run();

        delete kernel;
        return 0;
}
