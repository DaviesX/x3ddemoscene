#include <stdio.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_kernel.hpp>
#include <usr/usr_editor.hpp>

using namespace x3d;
using namespace x3d::usr;

extern "C" int kernel_main(int argc, char* argv[])
{

        //kernel_add_param("--test", "variable_memory_test0");
        //kernel_add_param("--test", "variable_memory_test1");
        kernel_add_param("--edit-mode", "");

        KernelEditor* kern_editor       = new KernelEditor();
        Kernel* kernel                  = new Kernel();
        kern_editor->register_editor_frontend(KernelEditor::GUI_FONTEND_GTK, nullptr);
        kern_editor->register_editor_backend(new EditorBackend());
        kernel->register_proxy(kern_editor);
        kernel->run();

        delete kernel;
        return 0;
}
