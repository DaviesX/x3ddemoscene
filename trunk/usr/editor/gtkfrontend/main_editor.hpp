#ifndef MAIN_EDITOR_H_INCLUDED
#define MAIN_EDITOR_H_INCLUDED


#include <usr/usr_editorfrontend.hpp>

/*
 * functions' declaration
 */
namespace x3d
{
namespace usr
{

GtkWidget *main_editor_get_region ( void );
void main_editor_draw_tmp_image ( void );

class MainEditor {
public:
        MainEditor(EditorGtkFrontend* frontend);
        ~MainEditor();

        bool            show(bool visible);
        GtkWindow*      get_window_widget();

        class MainEditorInt;
        class MainEditorInt*    pimpl;
};

}// namespace usr

}// namespce x3d


#endif // MAIN_EDITOR_H_INCLUDED
